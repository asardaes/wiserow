#ifndef WISEROW_R_VISITORS_H_
#define WISEROW_R_VISITORS_H_

#define R_NO_REMAP
#include <Rinternals.h>

namespace wiserow {

extern "C" {
    SEXP row_sums(SEXP metadata, SEXP data);
}

} // namespace wiserow

#endif // WISEROW_R_VISITORS_H_
