#ifndef WISEROW_NATESTWORKER_H_
#define WISEROW_NATESTWORKER_H_

#include <cstddef> // size_t

#include "../core.h"
#include "../utils.h"
#include "../visitors.h"

namespace wiserow {

class NATestWorker : public ParallelWorker
{
public:
    NATestWorker(const OperationMetadata& metadata,
                 const ColumnCollection& cc,
                 OutputWrapper<int>& ans,
                 const BulkBoolOp bulk_op);

    void work_row(std::size_t in_id, std::size_t out_id) override;

private:
    OutputWrapper<int>& ans_;
    const BulkBoolOp bulk_op_;
    const LogicalOperator op_;
    const NAVisitor na_visitor_;
};

} // namespace wiserow

#endif // WISEROW_NATESTWORKER_H_
