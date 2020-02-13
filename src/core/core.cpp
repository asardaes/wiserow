#include "../core.h"

#include "OperationMetadata.cpp"

#include "ColumnCollection.cpp"
#include "SurrogateColumn.cpp"

#include "DataFrameColumnCollection.cpp"
#include "MatrixColumnCollection.cpp"

#include "ParallelWorker.cpp"

#include "OutputWrapper.cpp"

namespace wiserow {

std::size_t output_length(const OperationMetadata& metadata, const ColumnCollection& col_collection) {
    if (metadata.rows.ptr) {
        return metadata.rows.len;
    }
    else if (metadata.rows.is_null) {
        return col_collection.nrow();
    }
    else {
        return 0;
    }
}

} // namespace wiserow
