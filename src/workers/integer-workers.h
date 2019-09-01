#ifndef WISEROW_INTEGERWORKERS_H_
#define WISEROW_INTEGERWORKERS_H_

#include <cstddef> // size_t
#include <memory>
#include <vector>

#include <Rcpp.h>

#include "../core.h"
#include "../utils.h"
#include "../visitors.h"

namespace wiserow {

// =================================================================================================

class IntOutputStrategy
{
public:
    virtual ~IntOutputStrategy() {}

    virtual void reset() {}
    virtual bool short_circuit() {
        return false;
    }

    virtual void apply(const std::size_t col, const bool flag) = 0;
    virtual int output(const std::size_t ncol, const bool any_na) = 0;

    virtual std::shared_ptr<IntOutputStrategy> clone() = 0;
};

// -------------------------------------------------------------------------------------------------

// Can also provide purely logical output
class CompBasedIntWorker : public ParallelWorker
{
public:
    CompBasedIntWorker(const OperationMetadata& metadata,
                       const ColumnCollection& cc,
                       OutputWrapper<int>& ans,
                       const SEXP& comp_op,
                       const Rcpp::List& target_vals);

    virtual void set_up_thread() override;
    virtual void clean_thread() override;
    virtual void work_row(std::size_t in_id, std::size_t out_id) override;

    std::shared_ptr<IntOutputStrategy> out_strategy;

private:
    static thread_local std::shared_ptr<IntOutputStrategy> thread_local_strategy_;

    const NAVisitor na_visitor_;

    OutputWrapper<int>& ans_;
    const CompOp comp_op_;

    std::vector<std::shared_ptr<BooleanVisitor>> visitors_;
    std::vector<bool> na_targets_;

    // sigh, for case TRUE == "TRUE"
    std::vector<char *> char_targets_;
    const ComparisonOperator comp_operator_;
};

// -------------------------------------------------------------------------------------------------

class BulkBoolStrategy : public IntOutputStrategy
{
public:
    BulkBoolStrategy(const BulkBoolOp& bb_op, const NaAction& na_action);

    virtual void reset() override;
    virtual bool short_circuit() override;

    virtual void apply(const std::size_t, const bool flag) override;
    virtual int output(const std::size_t ncol, const bool any_na) override;

    virtual std::shared_ptr<IntOutputStrategy> clone() override;

private:
    const BulkBoolOp bb_op_;
    const LogicalOperator logical_operator_;
    const NaAction na_action_;
    const bool init_;
    bool flag_;
};

} // namespace wiserow

#endif // WISEROW_INTEGERWORKERS_H_
