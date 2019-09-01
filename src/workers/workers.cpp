#include "../workers.h"

#include <complex>
#include <stdexcept> // logic_error
#include <string>

#include <boost/utility/string_ref.hpp>
#include <boost/variant/get.hpp>
#include <Rcpp.h>

namespace wiserow {

BoolTestWorker::BoolTestWorker(const OperationMetadata& metadata,
                               const ColumnCollection& cc,
                               OutputWrapper<int>& ans,
                               const BulkBoolOp bulk_op,
                               const std::shared_ptr<BooleanVisitor>& visitor)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , bulk_op_(bulk_op)
    , op_(bulk_op == BulkBoolOp::ALL ? BoolOp::AND : BoolOp::OR)
    , visitor_(visitor)
{ }

// -------------------------------------------------------------------------------------------------

void BoolTestWorker::work_row(std::size_t in_id, std::size_t out_id) {
    bool flag = bulk_op_ == BulkBoolOp::ALL ? true : false;

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        supported_col_t variant = col_collection_(in_id, j);
        flag = op_.apply(flag, boost::apply_visitor(*visitor_, variant));

        if (bulk_op_ == BulkBoolOp::ALL && !flag) {
            break;
        }
        else if (flag && (bulk_op_ == BulkBoolOp::ANY || bulk_op_ == BulkBoolOp::NONE)) {
            break;
        }
    }

    switch(bulk_op_) {
    case BulkBoolOp::ALL: {
        if (col_collection_.ncol() > 0) ans_[out_id] = flag;
        break;
    }
    case BulkBoolOp::ANY: {
        ans_[out_id] = flag;
        break;
    }
    case BulkBoolOp::NONE: {
        ans_[out_id] = !flag;
        break;
    }
    }
}

// =================================================================================================

NATestWorker::NATestWorker(const OperationMetadata& metadata,
                           const ColumnCollection& cc,
                           OutputWrapper<int>& ans,
                           const BulkBoolOp bulk_op)
    : BoolTestWorker(metadata, cc, ans, bulk_op, BooleanVisitorBuilder().is_na().build())
{ }

// =================================================================================================

InfTestWorker::InfTestWorker(const OperationMetadata& metadata,
                             const ColumnCollection& cc,
                             OutputWrapper<int>& ans,
                             const BulkBoolOp bulk_op)
    : BoolTestWorker(metadata, cc, ans, bulk_op, BooleanVisitorBuilder().is_inf().build())
{ }

// =================================================================================================

FiniteTestWorker::FiniteTestWorker(const OperationMetadata& metadata,
                                   const ColumnCollection& cc,
                                   OutputWrapper<int>& ans,
                                   const BulkBoolOp bulk_op)
    : BoolTestWorker(metadata, cc, ans, bulk_op, BooleanVisitorBuilder(BoolOp::AND, true).is_na(true).is_inf(true).build())
{ }

// =================================================================================================

struct target_traits {
    bool is_na;
    char* char_target;
};

target_traits get_target_traits(const SEXP& target) {
    switch(TYPEOF(target)) {
    case INTSXP:
        return { Rcpp::traits::is_na<INTSXP>(Rcpp::as<int>(target)), nullptr };
    case REALSXP:
        return { Rcpp::traits::is_na<REALSXP>(Rcpp::as<double>(target)), nullptr };
    case LGLSXP:
        return { Rcpp::traits::is_na<LGLSXP>(Rcpp::as<int>(target)), nullptr };
    case CPLXSXP:
        return { Rcpp::traits::is_na<CPLXSXP>(Rcpp::as<Rcomplex>(target)), nullptr };
    case STRSXP: {
        Rcpp::StringVector vec(target);

        if (Rcpp::traits::is_na<STRSXP>(vec[0])) {
            return { true, nullptr };
        }
        else {
            return { false, vec[0] };
        }
    }
    default:
        Rcpp::stop("[wiserow] target values for comparison can only be integers, doubles, logicals, complex numbers, or strings.");
    }
}

thread_local std::shared_ptr<IntOutputStrategy> CompBasedIntWorker::thread_local_strategy_ = nullptr;

// -------------------------------------------------------------------------------------------------

CompBasedIntWorker::CompBasedIntWorker(const OperationMetadata& metadata,
                                       const ColumnCollection& cc,
                                       OutputWrapper<int>& ans,
                                       const SEXP& comp_op,
                                       const Rcpp::List& target_vals)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , comp_op_(parse_comp_op(Rcpp::as<std::string>(comp_op)))
    , comp_operator_(comp_op_)
{
    for (R_xlen_t i = 0; i < target_vals.length(); i++) {
        target_traits tt = get_target_traits(target_vals[i]);
        visitors_.push_back(BooleanVisitorBuilder().compare(comp_op_, target_vals[i]).build());
        na_targets_.push_back(tt.is_na);
        char_targets_.push_back(tt.char_target);
    }
}

// -------------------------------------------------------------------------------------------------

void CompBasedIntWorker::set_up_thread() {
    if (!out_strategy) { // nocov start
        throw std::logic_error("Output strategy has not been set.");
    } // nocov end

    thread_local_strategy_ = out_strategy->clone();
}

// -------------------------------------------------------------------------------------------------

void CompBasedIntWorker::clean_thread() {
    if (thread_local_strategy_) {
        thread_local_strategy_->reset();
    }
}

// -------------------------------------------------------------------------------------------------

void CompBasedIntWorker::work_row(std::size_t in_id, std::size_t out_id) {
    if (!thread_local_strategy_) { // nocov start
        throw std::logic_error("Thread-local output strategy has not been set.");
    } // nocov end

    bool any_na = false;
    thread_local_strategy_->reset();

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        auto visitor = visitors_[j % visitors_.size()];
        bool na_target = na_targets_[j % na_targets_.size()];
        const char *char_target = char_targets_[j % char_targets_.size()];
        supported_col_t variant = col_collection_(in_id, j);

        if (!na_target) {
            bool is_na = boost::apply_visitor(na_visitor_, variant);
            if (is_na) {
                if (metadata.na_action == NaAction::PASS) any_na = true;
                continue;
            }
        }
        else if (comp_op_ != CompOp::EQ && comp_op_ != CompOp::NEQ) {
            // if target for comparison is NA but operator is not one of [==, !=], result is NA
            any_na = true;
            continue;
        }

        if (char_target && col_collection_[j]->is_logical()) {
            // tricky case when source is R-logical (with underlying int) that should be converted to string
            bool variant_bool = boost::get<int>(variant) != 0;
            boost::string_ref str_ref(char_target);
            thread_local_strategy_->apply(j, comp_operator_.apply(variant_bool, str_ref));
        }
        else {
            thread_local_strategy_->apply(j, boost::apply_visitor(*visitor, variant));
        }

        if (thread_local_strategy_->short_circuit()) {
            break;
        }
    }

    ans_[out_id] = thread_local_strategy_->output(col_collection_.ncol(), any_na);
}

// =================================================================================================

BulkBoolStrategy::BulkBoolStrategy(const BulkBoolOp& bb_op, const NaAction& na_action)
    : bb_op_(bb_op)
    , logical_operator_(bb_op == BulkBoolOp::ALL ? BoolOp::AND : BoolOp::OR)
    , na_action_(na_action)
    , init_(bb_op == BulkBoolOp::ALL ? true : false)
    , flag_(init_)
{ }

void BulkBoolStrategy::reset() {
    flag_ = init_;
}

bool BulkBoolStrategy::short_circuit() {
    // maybe don't break because R's all/any still check all values for NA when na.rm = FALSE
    if (na_action_ == NaAction::EXCLUDE) {
        if (bb_op_ == BulkBoolOp::ALL && !flag_) {
            return true;
        }
        else if (flag_ && (bb_op_ == BulkBoolOp::ANY || bb_op_ == BulkBoolOp::NONE)) {
            return true;
        }
    }

    return false;
}

void BulkBoolStrategy::apply(const std::size_t, const bool flag) {
    flag_ = logical_operator_.apply(flag_, flag);
}

int BulkBoolStrategy::output(const std::size_t ncol, const bool any_na) {
    switch(bb_op_) {
    case BulkBoolOp::ALL: {
        if (ncol > 0) {
            if (flag_ && any_na) {
                return NA_INTEGER;
            }
            else {
                return flag_;
            }
        }
        else {
            return 0;
        }
    }
    case BulkBoolOp::ANY: {
        if (!flag_ && any_na) {
            return NA_INTEGER;
        }
        else {
            return flag_;
        }
    }
    case BulkBoolOp::NONE: {
        if (!flag_ && any_na) {
            return NA_INTEGER;
        }
        else {
            return !flag_;
        }
    }
    }

    return NA_INTEGER; // nocov
}

std::shared_ptr<IntOutputStrategy> BulkBoolStrategy::clone() {
    return std::make_shared<BulkBoolStrategy>(this->bb_op_, this->na_action_);
}

} // namespace wiserow
