#include "../wiserow.h"

#include <cstddef> // size_t
#include <memory>
#include <string>

#include <Rcpp.h>

#include "../core.h"
#include "../workers.h"
#include "helpers.h"

namespace wiserow {

std::shared_ptr<OutputWrapper<int>> get_wrapper_ptr(const OperationMetadata& metadata, SEXP output) {
    switch(metadata.output_class) {
    case OutputClass::VECTOR: {
        if (metadata.output_mode == LGLSXP) {
            Rcpp::LogicalVector ans(output);
            return std::make_shared<VectorOutputWrapper<LGLSXP, int>>(ans);
        }
        else {
            Rcpp::IntegerVector ans(output);
            return std::make_shared<VectorOutputWrapper<INTSXP, int>>(ans);
        }
    }
    case OutputClass::LIST: {
        Rcpp::List ans(output);

        if (metadata.output_mode == LGLSXP) {
            return std::make_shared<ListOutputWrapper<LGLSXP, int>>(ans);
        }
        else {
            return std::make_shared<ListOutputWrapper<INTSXP, int>>(ans);
        }
    }
    case OutputClass::DATAFRAME: {
        if (metadata.output_mode == LGLSXP) {
            return std::make_shared<DataFrameOutputWrapper<LGLSXP, int>>(output);
        }
        else {
            return std::make_shared<DataFrameOutputWrapper<INTSXP, int>>(output);
        }
    }
    case OutputClass::MATRIX: {
        if (metadata.output_mode == LGLSXP) {
            return std::make_shared<MatrixOutputWrapper<LGLSXP, int>>(output);
        }
        else {
            return std::make_shared<MatrixOutputWrapper<INTSXP, int>>(output);
        }
    }
    default:
        Rcpp::stop("This operation does not support the chosen output class."); // nocov
    }
}

// =================================================================================================

template<typename Worker>
void visit_with_bulk_bool_op(SEXP metadata, SEXP data, SEXP output, const Rcpp::List& extras) {
    OperationMetadata metadata_(metadata);

    ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);
    std::size_t out_len = output_length(metadata_, col_collection);
    if (out_len == 0) return;

    std::string bulk_bool_op = Rcpp::as<std::string>(extras["match_type"]);

    std::shared_ptr<OutputWrapper<int>> wrapper_ptr = get_wrapper_ptr(metadata_, output);

    // always NaAction::Exclude to force short-circuit if appropriate
    if (bulk_bool_op == "all") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ALL, NaAction::EXCLUDE);
        Worker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else if (bulk_bool_op == "any") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ANY, NaAction::EXCLUDE);
        Worker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else if (bulk_bool_op == "none") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::NONE, NaAction::EXCLUDE);
        Worker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
}

// -------------------------------------------------------------------------------------------------

template<typename Worker>
void visit_with_match_type(SEXP metadata, SEXP data, SEXP output, const Rcpp::List extras) {
    std::string match_type = Rcpp::as<std::string>(extras["match_type"]);

    if (match_type == "which_first") {
        OperationMetadata metadata_(metadata);

        ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);
        std::size_t out_len = output_length(metadata_, col_collection);
        if (out_len == 0) return;

        std::shared_ptr<OutputWrapper<int>> wrapper_ptr = get_wrapper_ptr(metadata_, output);
        auto out_strategy = std::make_shared<WhichFirstStrategy>();
        Worker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else {
        visit_with_bulk_bool_op<Worker>(metadata, data, output, extras);
    }
}

// -------------------------------------------------------------------------------------------------

extern "C" SEXP row_finites(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    visit_with_match_type<FiniteTestWorker>(metadata, data, output, extras);
    return R_NilValue;
    END_RCPP
}

// -------------------------------------------------------------------------------------------------

extern "C" SEXP row_infs(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    visit_with_match_type<InfTestWorker>(metadata, data, output, extras);
    return R_NilValue;
    END_RCPP
}

// -------------------------------------------------------------------------------------------------

extern "C" SEXP row_nas(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    visit_with_match_type<NATestWorker>(metadata, data, output, extras);
    return R_NilValue;
    END_RCPP
}

// =================================================================================================

extern "C" SEXP row_compare(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    OperationMetadata metadata_(metadata);

    ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);
    std::size_t out_len = output_length(metadata_, col_collection);
    if (out_len == 0) return R_NilValue;

    Rcpp::List extras_(extras);
    std::string match_type = Rcpp::as<std::string>(extras_["match_type"]);
    SEXP comp_op = extras_["comp_op"];
    SEXP target_val = extras_["target_val"];

    std::shared_ptr<OutputWrapper<int>> wrapper_ptr = get_wrapper_ptr(metadata_, output);

    if (match_type == "all") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ALL, metadata_.na_action);
        CompBasedWorker<int> worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "any") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ANY, metadata_.na_action);
        CompBasedWorker<int> worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "none") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::NONE, metadata_.na_action);
        CompBasedWorker<int> worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "which_first") {
        auto out_strategy = std::make_shared<WhichFirstStrategy>();
        CompBasedWorker<int> worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val, out_strategy);
        parallel_for(worker);
    }

    return R_NilValue;
    END_RCPP
}

} // namespace wiserow
