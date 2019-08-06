#ifndef WISEROW_BOOLTESTWORKER_H_
#define WISEROW_BOOLTESTWORKER_H_

#include <cstddef> // size_t
#include <memory>

#include "../core.h"
#include "../utils.h"
#include "../visitors.h"

namespace wiserow {

class BoolTestWorker : public ParallelWorker
{
public:
    BoolTestWorker(const OperationMetadata& metadata,
                   const ColumnCollection& cc,
                   OutputWrapper<int>& ans,
                   const BulkBoolOp bulk_op,
                   const std::shared_ptr<BooleanVisitor>& visitor);

    virtual void work_row(std::size_t in_id, std::size_t out_id) override;

private:
    OutputWrapper<int>& ans_;
    const BulkBoolOp bulk_op_;
    const LogicalOperator op_;
    const std::shared_ptr<BooleanVisitor> visitor_;
};

} // namespace wiserow

#endif // WISEROW_BOOLTESTWORKER_H_
