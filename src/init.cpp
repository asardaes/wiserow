#include "wiserow.h"

#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#define CALLDEF(name, n) { "C_"#name, (DL_FUNC) &wiserow::name, n }

static const R_CallMethodDef callMethods[] = {
    CALLDEF(row_arith, 4),
    CALLDEF(row_compare, 4),
    CALLDEF(row_duplicated, 4),
    CALLDEF(row_extrema, 4),
    CALLDEF(row_finites, 4),
    CALLDEF(row_in, 4),
    CALLDEF(row_infs, 4),
    CALLDEF(row_means, 4),
    CALLDEF(row_nas, 4),
    {NULL, NULL, 0}
};

extern "C" void R_init_wiserow(DllInfo* info) {
    R_registerRoutines(info, NULL, callMethods, NULL, NULL);
    R_useDynamicSymbols(info, FALSE);
}
