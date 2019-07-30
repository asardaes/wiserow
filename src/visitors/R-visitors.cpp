#include "R-visitors.h"

#include <cstddef> // std::size_t
#include <string>
#include <typeinfo>
#include <typeindex>

#include "visitors.h"

namespace wiserow {

extern "C" SEXP row_sums(SEXP metadata, SEXP data) {
BEGIN_RCPP
    OperationMetadata metadata_(metadata);
    ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);

    if (metadata_.output_mode == typeid(int)) {
        Rcpp::IntegerVector ans(col_collection.nrow());
        SumVisitor<int> visitor_worker(metadata_, col_collection, &ans[0]);
        parallel_for(0, col_collection.nrow(), visitor_worker);
        return ans;
    }
    else if (metadata_.output_mode == typeid(double)) {
        Rcpp::NumericVector ans(col_collection.nrow());
        SumVisitor<double> visitor_worker(metadata_, col_collection, &ans[0]);
        parallel_for(0, col_collection.nrow(), visitor_worker);
        return ans;
    }
    else {
        Rcpp::stop("[wiserow] unsupported output mode: " + std::string(metadata_.output_mode.name()));
    }
END_RCPP
}

} // namespace wiserow
