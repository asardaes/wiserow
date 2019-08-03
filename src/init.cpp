#include "wiserow.h"

#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#define CALLDEF(name, n) { "C_"#name, (DL_FUNC) &wiserow::name, n }

static const R_CallMethodDef callMethods[] = {
    CALLDEF(row_sums, 3),
    {NULL, NULL, 0}
};

extern "C" void R_init_wiserow(DllInfo* info) {
    R_registerRoutines(info, NULL, callMethods, NULL, NULL);
    R_useDynamicSymbols(info, FALSE);
}
