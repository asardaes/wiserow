#ifndef WISEROW_H_
#define WISEROW_H_

#define R_NO_REMAP
#include <Rinternals.h>

namespace wiserow {

extern "C" {
    SEXP row_sums(SEXP metadata, SEXP data, SEXP output);
}

} // namespace wiserow

#endif // WISEROW_H_
