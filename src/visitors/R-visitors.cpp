#include "R-visitors.h"

#include <cstddef> // std::size_t
#include <string>
#include <typeinfo>
#include <typeindex>

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

    if (metadata_.output_mode == typeid(int)) {
        return row_sums_template<int, INTSXP>(metadata_, data);
    }
    else if (metadata_.output_mode == typeid(double)) {
        return row_sums_template<double, REALSXP>(metadata_, data);
    }
    else {
        Rcpp::stop("[wiserow] unsupported output mode: " + std::string(metadata_.output_mode.name()));
    }
END_RCPP
}

} // namespace wiserow
