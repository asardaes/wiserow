#ifndef WISEROW_INFTESTWORKER_H_
#define WISEROW_INFTESTWORKER_H_

#include "BoolTestWorker.h"

namespace wiserow {

class InfTestWorker : public BoolTestWorker
{
public:
    InfTestWorker(const OperationMetadata& metadata,
                  const ColumnCollection& cc,
                  OutputWrapper<int>& ans,
                  const BulkBoolOp bulk_op);
};

} // namespace wiserow

#endif // WISEROW_INFTESTWORKER_H_
