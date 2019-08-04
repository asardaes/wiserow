#ifndef WISEROW_BOOLEANUTILS_H_
#define WISEROW_BOOLEANUTILS_H_

namespace wiserow {

enum class BoolOp {
    AND,
    OR
};

enum class BulkCompOp {
    ALL,
    ANY,
    NONE
};

class LogicalOperator {
public:
    LogicalOperator(const BoolOp bool_op);

    bool apply(bool a, bool b) const;

private:
    const BoolOp bool_op_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANUTILS_H_
