#ifndef WISEROW_ARITHUTILS_H_
#define WISEROW_ARITHUTILS_H_

#include <string>

namespace wiserow {

enum class ArithOp {
    ADD,
    SUB,
    MUL,
    DIV
};

ArithOp parse_arith_op(const std::string& arith_op);

// =================================================================================================

// will NOT deal with NA
class ArithmeticOperator {
public:
    ArithmeticOperator(const ArithOp arith_op)
        : arith_op(arith_op)
    { }

    template<typename T, typename U>
    auto apply(const T a, const U b) const -> decltype(a + b) {
        switch(arith_op) {
        case ArithOp::ADD:
            return a + b;
        case ArithOp::SUB:
            return a - b;
        case ArithOp::MUL:
            return a * b;
        case ArithOp::DIV:
            return a / b;
        }

        return 0; // nocov
    }

    const ArithOp arith_op;
};

} // namespace wiserow

#endif // WISEROW_ARITHUTILS_H_
