#include "ArithUtils.h"

#include <stdexcept>

namespace wiserow {

ArithOp parse_arith_op(const std::string& arith_op) {
    if (arith_op == "+") {
        return ArithOp::ADD;
    }
    else if (arith_op == "-") {
        return ArithOp::SUB;
    }
    else if (arith_op == "*") {
        return ArithOp::MUL;
    }
    else if (arith_op == "/") {
        return ArithOp::DIV;
    }
    else {
        throw std::invalid_argument("[wiserow] invalid arithmetic operand."); // nocov - checked in R
    }
}

} // namespace wiserow
