#include "../workers.h"

#include <complex>
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
            Rcpp::CharacterVector val = Rcpp::as<Rcpp::CharacterVector>(vec[0]);
            char *val_ptr = (char *)(val[0]);
            return { false, val_ptr };
        }
    }
    default:
        Rcpp::stop("[wiserow] target values for comparison can only be integers, doubles, logicals, complex numbers, or strings.");
    }
}

// -------------------------------------------------------------------------------------------------

ComparisonWorker::ComparisonWorker(const OperationMetadata& metadata,
                                   const ColumnCollection& cc,
                                   OutputWrapper<int>& ans,
                                   const BulkBoolOp bulk_op,
                                   const SEXP& comp_op,
                                   const Rcpp::List& target_vals)
    : ParallelWorker(metadata, cc)
    , na_action_(metadata.na_action)
    , ans_(ans)
    , bulk_op_(bulk_op)
    , op_(bulk_op == BulkBoolOp::ALL ? BoolOp::AND : BoolOp::OR)
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

void ComparisonWorker::work_row(std::size_t in_id, std::size_t out_id) {
    bool flag = bulk_op_ == BulkBoolOp::ALL ? true : false;
    bool any_na = false;

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        auto visitor = visitors_[j % visitors_.size()];
        bool na_target = na_targets_[j % na_targets_.size()];
        char *char_target = char_targets_[j % char_targets_.size()];
        supported_col_t variant = col_collection_(in_id, j);

        if (!na_target) {
            bool is_na = boost::apply_visitor(na_visitor_, variant);
            if (is_na) {
                if (na_action_ == NaAction::PASS) {
                    any_na = true;
                }

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
            flag = op_.apply(flag, comp_operator_.apply(variant_bool, str_ref));
        }
        else {
            flag = op_.apply(flag, boost::apply_visitor(*visitor, variant));
        }

        // maybe don't break because R's all/any still check all values for NA when na.rm = FALSE
        if (na_action_ == NaAction::EXCLUDE) {
            if (bulk_op_ == BulkBoolOp::ALL && !flag) {
                break;
            }
            else if (flag && (bulk_op_ == BulkBoolOp::ANY || bulk_op_ == BulkBoolOp::NONE)) {
                break;
            }
        }
    }

    switch(bulk_op_) {
    case BulkBoolOp::ALL: {
        if (col_collection_.ncol() > 0) {
            if (flag && any_na) {
                ans_[out_id] = NA_INTEGER;
            }
            else {
                ans_[out_id] = flag;
            }
        }
        break;
    }
    case BulkBoolOp::ANY: {
        if (!flag && any_na) {
            ans_[out_id] = NA_INTEGER;
        }
        else {
            ans_[out_id] = flag;
        }
        break;
    }
    case BulkBoolOp::NONE: {
        if (!flag && any_na) {
            ans_[out_id] = NA_INTEGER;
        }
        else {
            ans_[out_id] = !flag;
        }
        break;
    }
    }
}

} // namespace wiserow
