#include "wiserow.h"

#include <cstddef> // std::size_t
#include <string>

#include "workers/workers.h"

namespace wiserow {

template<template<typename> class VT>
SEXP visit_into_vector(const char *fun_name, SEXP m, SEXP data) {
    BEGIN_RCPP
    OperationMetadata metadata(m);
    ColumnCollection col_collection = ColumnCollection::coerce(metadata, data);

    switch(metadata.output_mode) {
    case INTSXP: {
        Rcpp::IntegerVector ans(col_collection.nrow());
        VT<int> visitor_worker(metadata, col_collection, &ans[0]);
        parallel_for(0, col_collection.nrow(), visitor_worker);
        return ans;
    }
    case REALSXP: {
        Rcpp::NumericVector ans(col_collection.nrow());
        VT<double> visitor_worker(metadata, col_collection, &ans[0]);
        parallel_for(0, col_collection.nrow(), visitor_worker);
        return ans;
    }
    default: {
        Rcpp::stop("[wiserow] %s can only return integers or doubles.", fun_name);
    }
    }
    END_RCPP
}

extern "C" SEXP row_sums(SEXP metadata, SEXP data) {
    return visit_into_vector<RowSumWorker>("row_sums", metadata, data);
}

} // namespace wiserow
