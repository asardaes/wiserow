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

template<template<typename> class Worker>
void visit_into_numeric_vector(const char* fun_name, const OperationMetadata& metadata, SEXP data, SEXP output) {
    ColumnCollection col_collection = ColumnCollection::coerce(metadata, data);
    std::size_t out_len = output_length(metadata, col_collection);

    switch(metadata.output_mode) {
    case INTSXP: {
        if (out_len == 0) break;

        VectorOutputWrapper<INTSXP, int> wrapper(output);
        Worker<int> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    case REALSXP: {
        if (out_len == 0) break;

        VectorOutputWrapper<REALSXP, double> wrapper(output);
        Worker<double> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    case LGLSXP: {
        if (out_len == 0) break;

        VectorOutputWrapper<LGLSXP, int> wrapper(output);
        Worker<int> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    case CPLXSXP: {
        if (out_len == 0) break;

        VectorOutputWrapper<CPLXSXP, std::complex<double>> wrapper(output);
        Worker<std::complex<double>> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    default: {
        Rcpp::stop("[wiserow] %s can only return integers, doubles, logicals, or complex numbers.", fun_name);
    }
    }
}

// -------------------------------------------------------------------------------------------------

template<template<typename> class Worker>
void visit_into_enlisted_numeric_vector(const char* fun_name, const OperationMetadata& metadata, SEXP data, SEXP output) {
    ColumnCollection col_collection = ColumnCollection::coerce(metadata, data);
    std::size_t out_len = output_length(metadata, col_collection);

    switch(metadata.output_mode) {
    case INTSXP: {
        if (out_len == 0) break;

        ListOutputWrapper<INTSXP, int> wrapper(output);
        Worker<int> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    case REALSXP: {
        if (out_len == 0) break;

        ListOutputWrapper<REALSXP, double> wrapper(output);
        Worker<double> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    case CPLXSXP: {
        if (out_len == 0) break;

        ListOutputWrapper<CPLXSXP, std::complex<double>> wrapper(output);
        Worker<std::complex<double>> worker(metadata, col_collection, wrapper);

        parallel_for(worker);
        break;
    }
    default: {
        Rcpp::stop("[wiserow] %s can only return integers, doubles, or complex numbers.", fun_name);
    }
    }
}

// =================================================================================================

template<template<typename> class Worker>
SEXP visit_into_numeric(const char* fun_name, SEXP m, SEXP data, SEXP output) {
    OperationMetadata metadata(m);

    switch(metadata.output_class) {
    case OutputClass::VECTOR: {
        visit_into_numeric_vector<Worker>(fun_name, metadata, data, output);
        break;
    }
    case OutputClass::LIST: {
        visit_into_enlisted_numeric_vector<Worker>(fun_name, metadata, data, output);
        break;
    }
    default: { // nocov start
        Rcpp::stop("[wiserow] (visit_into_numeric) this should never happen.");
    } // nocov end
    }

    return R_NilValue;
}

} // namespace wiserow

#endif // WISEROW_NUMERICVISIT_H_
