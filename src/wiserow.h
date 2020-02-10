#ifndef WISEROW_H_
#define WISEROW_H_

#define R_NO_REMAP
#include <Rinternals.h>

namespace wiserow {

extern "C" {
    SEXP row_arith(SEXP metadata, SEXP data, SEXP output, SEXP extras);
    SEXP row_compare(SEXP metadata, SEXP data, SEXP output, SEXP extras);
    SEXP row_duplicated(SEXP metadata, SEXP data, SEXP output, SEXP extras);
    SEXP row_extrema(SEXP metadata, SEXP data, SEXP output, SEXP extras);
    SEXP row_finites(SEXP metadata, SEXP data, SEXP output, SEXP extras);
    SEXP row_in(SEXP metadata, SEXP data, SEXP output, SEXP extras);
    SEXP row_infs(SEXP metadata, SEXP data, SEXP output, SEXP extras);
    SEXP row_means(SEXP metadata, SEXP data, SEXP output, SEXP extras);
    SEXP row_nas(SEXP metadata, SEXP data, SEXP output, SEXP extras);
}

} // namespace wiserow

#endif // WISEROW_H_
