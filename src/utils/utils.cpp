#include "BooleanUtils.h"

namespace wiserow {

LogicalOperator::LogicalOperator(const BoolOp bool_op)
    : bool_op_(bool_op)
{ }

bool LogicalOperator::apply(bool a, bool b) const {
    switch(bool_op_) {
    case BoolOp::AND:
        return a && b;
    case BoolOp::OR:
        return a || b;
    }

    return false; // nocov
}

} // namespace wiserow
