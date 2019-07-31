#include "R-visitors.h"

#include <cstddef> // std::size_t
#include <string>

#include "visitors.h"

namespace wiserow {

template<typename T, int RT>
Rcpp::Vector<RT> row_sums_template(const OperationMetadata& metadata, SEXP data) {
    ColumnCollection col_collection = ColumnCollection::coerce(metadata, data);
    Rcpp::Vector<RT> ans(col_collection.nrow());
    SumVisitor<T> visitor_worker(metadata, col_collection, &ans[0]);
    parallel_for(0, col_collection.nrow(), visitor_worker);
    return ans;
}

extern "C" SEXP row_sums(SEXP metadata, SEXP data) {
BEGIN_RCPP
    OperationMetadata metadata_(metadata);

    switch(metadata_.output_mode) {
    case INTSXP: {
        return row_sums_template<int, INTSXP>(metadata_, data);
    }
    case REALSXP: {
        return row_sums_template<double, REALSXP>(metadata_, data);
    }
    default: {
        Rcpp::stop("[wiserow] row_sums can only return integers or doubles.");
    }
    }
END_RCPP
}

} // namespace wiserow
