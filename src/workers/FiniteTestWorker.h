#ifndef WISEROW_FINITETESTWORKER_H_
#define WISEROW_FINITETESTWORKER_H_

#include "BoolTestWorker.h"

namespace wiserow {

class FiniteTestWorker : public BoolTestWorker
{
public:
    FiniteTestWorker(const OperationMetadata& metadata,
                     const ColumnCollection& cc,
                     OutputWrapper<int>& ans,
                     const BulkBoolOp bulk_op);
};

} // namespace wiserow

#endif // WISEROW_FINITETESTWORKER_H_
