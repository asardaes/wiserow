#include "NATestWorker.h"

namespace wiserow {

// =================================================================================================

NATestWorker::NATestWorker(const OperationMetadata& metadata,
                           const ColumnCollection& cc,
                           OutputWrapper<int>& ans,
                           const BulkBoolOp bulk_op)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , bulk_op_(bulk_op)
    , op_(bulk_op == BulkBoolOp::ALL ? BoolOp::AND : BoolOp::OR)
{ }

void NATestWorker::work_row(std::size_t in_id, std::size_t out_id) {
    bool flag = bulk_op_ == BulkBoolOp::ALL ? true : false;

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        bool is_na = boost::apply_visitor(na_visitor_, col_collection_(in_id, j));
        flag = op_.apply(flag, is_na);

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

} // namespace wiserow
