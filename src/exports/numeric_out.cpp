#include "../wiserow.h"

#include <Rcpp.h>

#include "../core.h"
#include "../workers.h"
#include "numeric_visit.h"

namespace wiserow {

extern "C" SEXP row_sums(SEXP metadata, SEXP data, SEXP output) {
    BEGIN_RCPP
    return visit_into_numeric<RowSumsWorker>("row_sums", metadata, data, output);
    END_RCPP
}

} // namespace wiserow
