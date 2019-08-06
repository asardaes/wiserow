#ifndef WISEROW_NATESTWORKER_H_
#define WISEROW_NATESTWORKER_H_

#include "BoolTestWorker.h"

namespace wiserow {

class NATestWorker : public BoolTestWorker
{
public:
    NATestWorker(const OperationMetadata& metadata,
                 const ColumnCollection& cc,
                 OutputWrapper<int>& ans,
                 const BulkBoolOp bulk_op);
};

} // namespace wiserow

#endif // WISEROW_NATESTWORKER_H_
