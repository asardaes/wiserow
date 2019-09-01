#ifndef WISEROW_WORKERSTRATEGIES_H_
#define WISEROW_WORKERSTRATEGIES_H_

#include <cstddef> // size_t
#include <memory>

#include "../core.h"
#include "../utils.h"

namespace wiserow {

template<typename T>
class OutputStrategy : public WorkerThreadLocal
{
public:
    virtual ~OutputStrategy() {}

    virtual void reinit() {}
    virtual bool short_circuit() {
        return false;
    }

    virtual void apply(const std::size_t col, const supported_col_t& variant, const T match_flag) = 0;
    virtual T output(const std::size_t ncol, const bool any_na) = 0;

    virtual std::shared_ptr<OutputStrategy<T>> clone() = 0;
};

// -------------------------------------------------------------------------------------------------

class BulkBoolStrategy : public OutputStrategy<int>
{
public:
    BulkBoolStrategy(const BulkBoolOp& bb_op, const NaAction& na_action);

    virtual void reinit() override;
    virtual bool short_circuit() override;

    virtual void apply(const std::size_t, const supported_col_t&, const int match_flag) override;
    virtual int output(const std::size_t ncol, const bool any_na) override;

    virtual std::shared_ptr<OutputStrategy<int>> clone() override;

private:
    const BulkBoolOp bb_op_;
    const LogicalOperator logical_operator_;
    const NaAction na_action_;
    const bool init_;
    bool flag_;
};

} // namespace wiserow

#endif // WISEROW_WORKERSTRATEGIES_H_
