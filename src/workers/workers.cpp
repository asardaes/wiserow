#include "../workers.h"

#include <complex>

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

ParallelWorker::thread_local_ptr BoolTestWorker::work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr) {
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

    return nullptr;
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
