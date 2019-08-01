#include "R-visitors.h"

#include <cstddef> // std::size_t
#include <string>

#include "visitors.h"

namespace wiserow {

template<const char *fun, template<typename> class VT>
SEXP visit_into_vector(SEXP m, SEXP data) {
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
        Rcpp::stop("[wiserow] %s can only return integers or doubles.", fun);
    }
    }
    END_RCPP
}

static const char row_sums_name[] = "row_sums";

extern "C" SEXP row_sums(SEXP metadata, SEXP data) {
    return visit_into_vector<row_sums_name, SumVisitor>(metadata, data);
}

} // namespace wiserow
