#include "wiserow.h"

#include <complex>
#include <cstddef> // std::size_t
#include <string>

#include "workers/workers.h"

namespace wiserow {

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

template<template<typename> class Worker>
SEXP visit_into_numeric_vector(const char* fun_name, SEXP m, SEXP data) {
    BEGIN_RCPP
    OperationMetadata metadata(m);
    ColumnCollection col_collection = ColumnCollection::coerce(metadata, data);
    std::size_t out_len = output_length(metadata, col_collection);

    switch(metadata.output_mode) {
    case INTSXP: {
        Rcpp::IntegerVector ans(out_len);
        if (out_len == 0) return ans;

        Worker<int> worker(metadata, col_collection, &ans[0]);
        parallel_for(worker);
        return ans;
    }
    case REALSXP: {
        Rcpp::NumericVector ans(out_len);
        if (out_len == 0) return ans;

        Worker<double> worker(metadata, col_collection, &ans[0]);
        parallel_for(worker);
        return ans;
    }
    case CPLXSXP: {
        Rcpp::ComplexVector ans(out_len);
        if (out_len == 0) return ans;

        auto ptr = reinterpret_cast<std::complex<double> *>(&ans[0]);
        Worker<std::complex<double>> worker(metadata, col_collection, ptr);
        parallel_for(worker);
        return ans;
    }
    default: {
        Rcpp::stop("[wiserow] %s can only return integers, doubles or complex numbers.", fun_name);
    }
    }
    END_RCPP
}

extern "C" SEXP row_sums(SEXP metadata, SEXP data) {
    return visit_into_numeric_vector<RowSumsWorker>("row_sums", metadata, data);
}

} // namespace wiserow
