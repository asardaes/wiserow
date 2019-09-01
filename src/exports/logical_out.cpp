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
        Rcpp::LogicalVector ans(output);
        return std::make_shared<VectorOutputWrapper<LGLSXP, int>>(ans);
    }
    case OutputClass::LIST: {
        Rcpp::List ans(output);
        return std::make_shared<ListOutputWrapper<LGLSXP, int>>(ans);
    }
    }

    return nullptr; // nocov
}

// =================================================================================================

template<typename Worker>
void visit_with_bulk_bool_op(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    OperationMetadata metadata_(metadata);

    ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);
    std::size_t out_len = output_length(metadata_, col_collection);
    if (out_len == 0) return;

    Rcpp::List extras_(extras);
    std::string bulk_bool_op = Rcpp::as<std::string>(extras_["bulk_bool_op"]);

    std::shared_ptr<OutputWrapper<int>> wrapper_ptr = get_wrapper_ptr(metadata_, output);

    if (bulk_bool_op == "all") {
        Worker worker(metadata_, col_collection, *wrapper_ptr, BulkBoolOp::ALL);
        parallel_for(worker);
    }
    else if (bulk_bool_op == "any") {
        Worker worker(metadata_, col_collection, *wrapper_ptr, BulkBoolOp::ANY);
        parallel_for(worker);
    }
    else if (bulk_bool_op == "none") {
        Worker worker(metadata_, col_collection, *wrapper_ptr, BulkBoolOp::NONE);
        parallel_for(worker);
    }
}

// -------------------------------------------------------------------------------------------------

extern "C" SEXP row_finites(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    visit_with_bulk_bool_op<FiniteTestWorker>(metadata, data, output, extras);
    return R_NilValue;
    END_RCPP
}

// -------------------------------------------------------------------------------------------------

extern "C" SEXP row_infs(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    visit_with_bulk_bool_op<InfTestWorker>(metadata, data, output, extras);
    return R_NilValue;
    END_RCPP
}

// -------------------------------------------------------------------------------------------------

extern "C" SEXP row_nas(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    visit_with_bulk_bool_op<NATestWorker>(metadata, data, output, extras);
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
    std::string bulk_bool_op = Rcpp::as<std::string>(extras_["bulk_bool_op"]);
    SEXP comp_op = extras_["comp_op"];
    SEXP target_val = extras_["target_val"];

    std::shared_ptr<OutputWrapper<int>> wrapper_ptr = get_wrapper_ptr(metadata_, output);

    if (bulk_bool_op == "all") {
        CompBasedIntWorker worker(metadata_, col_collection, *wrapper_ptr,comp_op, target_val);
        worker.out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ALL, metadata_.na_action);
        parallel_for(worker);
    }
    else if (bulk_bool_op == "any") {
        CompBasedIntWorker worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val);
        worker.out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ANY, metadata_.na_action);
        parallel_for(worker);
    }
    else if (bulk_bool_op == "none") {
        CompBasedIntWorker worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val);
        worker.out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::NONE, metadata_.na_action);
        parallel_for(worker);
    }

    return R_NilValue;
    END_RCPP
}

} // namespace wiserow
