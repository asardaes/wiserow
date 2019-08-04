#ifndef WISEROW_EXPORTSHELPERS_H_
#define WISEROW_EXPORTSHELPERS_H_

#include "../core/OperationMetadata.h"
#include "../core/columns.h"

namespace wiserow {

// =================================================================================================

std::size_t output_length(const OperationMetadata& metadata, const ColumnCollection& col_collection);

} // namespace wiserow

#endif // WISEROW_EXPORTSHELPERS_H_
