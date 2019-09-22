#ifndef WISEROW_NUMERICVISIT_H_
#define WISEROW_NUMERICVISIT_H_

#include <complex>
#include <cstddef> // size_t

#include <Rcpp.h>

#include "../core.h"
#include "../workers.h"
#include "helpers.h"

namespace wiserow {

// =================================================================================================

template<typename Worker, typename Wrapper>
void visit_into_numeric(const OperationMetadata& metadata,
                        const ColumnCollection& col_collection,
                        SEXP output,
                        const std::size_t out_len,
                        SEXP extras)
{
    if (out_len == 0) return;

    Wrapper wrapper(output);
    Worker worker(metadata, col_collection, wrapper, extras);
    parallel_for(worker);
}

// -------------------------------------------------------------------------------------------------

template<template<typename> class Worker, template<int, typename> class Wrapper>
void visit_into_numeric(const char* fun_name, const OperationMetadata& metadata, SEXP data, SEXP output, SEXP extras) {
    ColumnCollection col_collection = ColumnCollection::coerce(metadata, data);
    std::size_t out_len = output_length(metadata, col_collection);

    switch(metadata.output_mode) {
    case INTSXP:
        visit_into_numeric<Worker<int>, Wrapper<INTSXP, int>>(metadata, col_collection, output, out_len, extras);
        break;
    case REALSXP:
        visit_into_numeric<Worker<double>, Wrapper<REALSXP, double>>(metadata, col_collection, output, out_len, extras);
        break;
    case LGLSXP:
        visit_into_numeric<Worker<int>, Wrapper<LGLSXP, int>>(metadata, col_collection, output, out_len, extras);
        break;
    case CPLXSXP:
        visit_into_numeric<Worker<std::complex<double>>, Wrapper<CPLXSXP, std::complex<double>>>(
                metadata, col_collection, output, out_len, extras);
        break;
    default:
        Rcpp::stop("[wiserow] %s can only return integers, doubles, logicals, or complex numbers.", fun_name);
    }
}

// -------------------------------------------------------------------------------------------------

template<template<typename> class Worker>
SEXP visit_into_numeric(const char* fun_name, SEXP m, SEXP data, SEXP output, SEXP extras) {
    OperationMetadata metadata(m);

    switch(metadata.output_class) {
    case OutputClass::VECTOR:
        visit_into_numeric<Worker, VectorOutputWrapper>(fun_name, metadata, data, output, extras);
        break;
    case OutputClass::LIST:
        visit_into_numeric<Worker, ListOutputWrapper>(fun_name, metadata, data, output, extras);
        break;
    case OutputClass::DATAFRAME:
        visit_into_numeric<Worker, DataFrameOutputWrapper>(fun_name, metadata, data, output, extras);
        break;
    case OutputClass::MATRIX:
        visit_into_numeric<Worker, MatrixOutputWrapper>(fun_name, metadata, data, output, extras);
        break;
    default: // nocov start
        Rcpp::stop("[wiserow] (visit_into_numeric) this should never happen."); // nocov end
    }

    return R_NilValue;
}

} // namespace wiserow

#endif // WISEROW_NUMERICVISIT_H_
