#include "../workers.h"

#include <complex>
#include <memory>

#include <Rcpp.h>

namespace wiserow {

BoolTestWorker::BoolTestWorker(const OperationMetadata& metadata,
                               const ColumnCollection& cc,
                               OutputWrapper<int>& ans,
                               const std::shared_ptr<BooleanVisitor>& visitor,
                               const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , visitor_(visitor)
    , out_strategy_(out_strategy)
{ }

// -------------------------------------------------------------------------------------------------

ParallelWorker::thread_local_ptr BoolTestWorker::work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) {
    std::shared_ptr<OutputStrategy<int>> thread_local_strategy =
            t_local ? std::static_pointer_cast<OutputStrategy<int>>(t_local) : out_strategy_->clone();

    thread_local_strategy->reinit();

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        supported_col_t variant = col_collection_(in_id, j);
        thread_local_strategy->apply(j, variant, boost::apply_visitor(*visitor_, variant));

        if (thread_local_strategy->short_circuit()) {
            break;
        }
    }

    ans_[out_id] = thread_local_strategy->output(col_collection_.ncol(), false);
    return thread_local_strategy;
}

// =================================================================================================

NATestWorker::NATestWorker(const OperationMetadata& metadata,
                           const ColumnCollection& cc,
                           OutputWrapper<int>& ans,
                           const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : BoolTestWorker(metadata, cc, ans, BooleanVisitorBuilder().is_na().build(), out_strategy)
{ }

// =================================================================================================

InfTestWorker::InfTestWorker(const OperationMetadata& metadata,
                             const ColumnCollection& cc,
                             OutputWrapper<int>& ans,
                             const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : BoolTestWorker(metadata, cc, ans, BooleanVisitorBuilder().is_inf().build(), out_strategy)
{ }

// =================================================================================================

FiniteTestWorker::FiniteTestWorker(const OperationMetadata& metadata,
                                   const ColumnCollection& cc,
                                   OutputWrapper<int>& ans,
                                   const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : BoolTestWorker(metadata, cc, ans, BooleanVisitorBuilder(BoolOp::AND, true).is_na(true).is_inf(true).build(), out_strategy)
{ }

// =================================================================================================

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

// =================================================================================================

BulkBoolStrategy::BulkBoolStrategy(const BulkBoolOp& bb_op, const NaAction& na_action)
    : bb_op_(bb_op)
    , logical_operator_(bb_op == BulkBoolOp::ALL ? BoolOp::AND : BoolOp::OR)
    , na_action_(na_action)
    , init_(bb_op == BulkBoolOp::ALL ? true : false)
    , flag_(init_)
{ }

void BulkBoolStrategy::reinit() {
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

void BulkBoolStrategy::apply(const std::size_t, const supported_col_t&, const int match_flag) {
    flag_ = logical_operator_.apply(flag_, static_cast<bool>(match_flag));
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

std::shared_ptr<OutputStrategy<int>> BulkBoolStrategy::clone() {
    return std::make_shared<BulkBoolStrategy>(this->bb_op_, this->na_action_);
}

} // namespace wiserow
