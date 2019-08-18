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

std::shared_ptr<BooleanVisitor> get_comp_visitor(const CompOp& comp_op,
                                                 const SEXP& target_val,
                                                 bool& na_target,
                                                 char*& char_target)
{
    bool negate = comp_op == CompOp::EQ ? false : true;

    switch(TYPEOF(target_val)) {
    case INTSXP: {
        Rcpp::IntegerVector vec(target_val);
        int val = vec[0];

        if (Rcpp::IntegerVector::is_na(val)) {
            na_target = true;
            return BooleanVisitorBuilder().is_na(negate).build();
        }
        else {
            return BooleanVisitorBuilder(BoolOp::AND, true).compare(comp_op, val).build();
        }
    }
    case REALSXP: {
        Rcpp::NumericVector vec(target_val);
        double val = vec[0];

        if (Rcpp::NumericVector::is_na(val)) {
            na_target = true;
            return BooleanVisitorBuilder().is_na(negate).build();
        }
        else {
            return BooleanVisitorBuilder(BoolOp::AND, true).compare(comp_op, val).build();
        }
    }
    case LGLSXP: {
        Rcpp::LogicalVector vec(target_val);
        int val = vec[0];

        if (Rcpp::LogicalVector::is_na(val)) {
            na_target = true;
            return BooleanVisitorBuilder().is_na(negate).build();
        }
        else {
            return BooleanVisitorBuilder(BoolOp::AND, true).compare(comp_op, val != 0).build();
        }
    }
    case CPLXSXP: {
        Rcpp::ComplexVector vec(target_val);
        Rcomplex val = vec[0];

        if (Rcpp::ComplexVector::is_na(val)) {
            na_target = true;
            return BooleanVisitorBuilder().is_na(negate).build();
        }
        else {
            return BooleanVisitorBuilder(BoolOp::AND, true).compare(comp_op, std::complex<double>(val.r, val.i)).build();
        }
    }
    case STRSXP: {
        Rcpp::StringVector vec(target_val);

        if (Rcpp::traits::is_na<STRSXP>(vec[0])) {
            na_target = true;
            return BooleanVisitorBuilder().is_na(negate).build();
        }
        else {
            Rcpp::CharacterVector val = Rcpp::as<Rcpp::CharacterVector>(vec[0]);
            char *val_ptr = (char *)(val[0]);
            boost::string_ref str_ref(val_ptr);

            char_target = val_ptr;
            return BooleanVisitorBuilder(BoolOp::AND, true).compare(comp_op, str_ref).build();
        }
    }
    default: {
        Rcpp::stop("[wiserow] target values for comparison can only be integers, doubles, logicals, complex numbers, or strings.");
    }
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
        bool na_target = false;
        char *char_target = nullptr;
        visitors_.push_back(get_comp_visitor(comp_op_, target_vals[i], na_target, char_target));
        na_targets_.push_back(na_target);
        char_targets_.push_back(char_target);
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
