#ifndef WISEROW_DATACOERCER_H_
#define WISEROW_DATACOERCER_H_

#include <Rcpp.h>

#include "Metadata.h"
#include "column-collections.h"

namespace wiserow {

class DataCoercer
{
public:
    static ColumnCollection coerce(Metadata metadata, SEXP data);
};

} // namespace wiserow

#endif // WISEROW_DATACOERCER_H_
