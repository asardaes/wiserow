#include "../workers.h"

#include "BoolTestWorker.h"

namespace wiserow {

BoolTestWorker::BoolTestWorker(const OperationMetadata& metadata,
                               const ColumnCollection& cc,
                               OutputWrapper<int>& ans,
                               const BulkBoolOp bulk_op,
                               const std::shared_ptr<BooleanVisitor>& visitor)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , bulk_op_(bulk_op)
    , op_(bulk_op == BulkBoolOp::ALL ? BoolOp::AND : BoolOp::OR)
    , visitor_(visitor)
{ }

void BoolTestWorker::work_row(std::size_t in_id, std::size_t out_id) {
    bool flag = bulk_op_ == BulkBoolOp::ALL ? true : false;

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        flag = op_.apply(flag, boost::apply_visitor(*visitor_, col_collection_(in_id, j)));

        if (bulk_op_ == BulkBoolOp::ALL && !flag) {
            break;
        }
        else if (flag && (bulk_op_ == BulkBoolOp::ANY || bulk_op_ == BulkBoolOp::NONE)) {
            break;
        }
    }

    switch(bulk_op_) {
    case BulkBoolOp::ALL: {
        if (col_collection_.ncol() > 0) ans_[out_id] = flag;
        break;
    }
    case BulkBoolOp::ANY: {
        ans_[out_id] = flag;
        break;
    }
    case BulkBoolOp::NONE: {
        ans_[out_id] = !flag;
        break;
    }
    }
}

// =================================================================================================

NATestWorker::NATestWorker(const OperationMetadata& metadata,
                           const ColumnCollection& cc,
                           OutputWrapper<int>& ans,
                           const BulkBoolOp bulk_op)
    : BoolTestWorker(metadata, cc, ans, bulk_op, BooleanVisitorBuilder().is_na().build())
{ }

} // namespace wiserow
