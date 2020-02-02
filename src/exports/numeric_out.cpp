#include "../wiserow.h"

#include <Rcpp.h>

#include "../workers.h"
#include "numeric_visit.h"

namespace wiserow {

extern "C" SEXP row_arith(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    return visit_into_numeric<RowArithWorker>("row_sums", metadata, data, output, extras);
    END_RCPP
}

extern "C" SEXP row_means(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    return visit_into_numeric<RowMeansWorker>("row_means", metadata, data, output, extras);
    END_RCPP
}

} // namespace wiserow
