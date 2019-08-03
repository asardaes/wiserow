#include "wiserow.h"

#include <complex>
#include <cstddef> // size_t
#include <string>

#include <Rcpp.h>

#include "workers/workers.h"

namespace wiserow {

// =================================================================================================

std::size_t output_length(const OperationMetadata& metadata, const ColumnCollection& col_collection) {
    if (metadata.rows.ptr) {
        return metadata.rows.len;
    }
    else if (metadata.rows.is_null) {
        return col_collection.nrow();
    }
    else {
        return 0;
    }
}

// -------------------------------------------------------------------------------------------------

template<template<typename> class Worker>
void visit_into_numeric_vector(const char* fun_name, const OperationMetadata& metadata, SEXP data, SEXP output) {
    ColumnCollection col_collection = ColumnCollection::coerce(metadata, data);
    std::size_t out_len = output_length(metadata, col_collection);

    switch(metadata.output_mode) {
    case INTSXP: {
        if (out_len == 0) break;

        Rcpp::IntegerVector ans(output);
        VectorOutputWrapper<INTSXP, int> wrapper(ans);
        Worker<int> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    case REALSXP: {
        if (out_len == 0) break;

        Rcpp::NumericVector ans(output);
        VectorOutputWrapper<REALSXP, double> wrapper(ans);
        Worker<double> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    case CPLXSXP: {
        if (out_len == 0) break;

        Rcpp::ComplexVector ans(output);
        VectorOutputWrapper<CPLXSXP, std::complex<double>> wrapper(ans);
        Worker<std::complex<double>> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    default: {
        Rcpp::stop("[wiserow] %s can only return integers, doubles, or complex numbers.", fun_name);
    }
    }
}

// -------------------------------------------------------------------------------------------------

template<template<typename> class Worker>
void visit_into_enlisted_numeric_vector(const char* fun_name, const OperationMetadata& metadata, SEXP data, SEXP output) {
    ColumnCollection col_collection = ColumnCollection::coerce(metadata, data);
    std::size_t out_len = output_length(metadata, col_collection);
    Rcpp::List ans(output);

    switch(metadata.output_mode) {
    case INTSXP: {
        if (out_len == 0) break;

        ListOutputWrapper<INTSXP, int> wrapper(ans);
        Worker<int> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    case REALSXP: {
        if (out_len == 0) break;

        ListOutputWrapper<REALSXP, double> wrapper(ans);
        Worker<double> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    case CPLXSXP: {
        if (out_len == 0) break;

        ListOutputWrapper<CPLXSXP, std::complex<double>> wrapper(ans);
        Worker<std::complex<double>> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    default: {
        Rcpp::stop("[wiserow] %s can only return integers, doubles, or complex numbers.", fun_name);
    }
    }
}

// -------------------------------------------------------------------------------------------------

template<template<typename> class Worker>
SEXP visit(const char* fun_name, SEXP m, SEXP data, SEXP output) {
    BEGIN_RCPP
    OperationMetadata metadata(m);

    switch(metadata.output_class) {
    case OutputClass::vector: {
        visit_into_numeric_vector<Worker>(fun_name, metadata, data, output);
        break;
    }
    case OutputClass::list: {
        visit_into_enlisted_numeric_vector<Worker>(fun_name, metadata, data, output);
        break;
    }
    default: { // nocov start
        Rcpp::stop("[wiserow] (visit) this should never happen.");
    } // nocov end
    }

    return R_NilValue;
    END_RCPP
}

// =================================================================================================

extern "C" SEXP row_sums(SEXP metadata, SEXP data, SEXP output) {
    return visit<RowSumsWorker>("row_sums", metadata, data, output);
}

} // namespace wiserow
