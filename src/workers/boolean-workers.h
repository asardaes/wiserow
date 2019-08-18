#ifndef WISEROW_BOOLEANWORKERS_H_
#define WISEROW_BOOLEANWORKERS_H_

#include <cstddef> // size_t
#include <memory>
#include <vector>

#include <Rcpp.h>

#include "../core.h"
#include "../utils.h"
#include "../visitors.h"

namespace wiserow {

// =================================================================================================

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

// =================================================================================================

class NATestWorker : public BoolTestWorker
{
public:
    NATestWorker(const OperationMetadata& metadata,
                 const ColumnCollection& cc,
                 OutputWrapper<int>& ans,
                 const BulkBoolOp bulk_op);
};

// =================================================================================================

class InfTestWorker : public BoolTestWorker
{
public:
    InfTestWorker(const OperationMetadata& metadata,
                  const ColumnCollection& cc,
                  OutputWrapper<int>& ans,
                  const BulkBoolOp bulk_op);
};

// =================================================================================================

class FiniteTestWorker : public BoolTestWorker
{
public:
    FiniteTestWorker(const OperationMetadata& metadata,
                     const ColumnCollection& cc,
                     OutputWrapper<int>& ans,
                     const BulkBoolOp bulk_op);
};

// =================================================================================================

class ComparisonWorker : public ParallelWorker
{
public:
    ComparisonWorker(const OperationMetadata& metadata,
                     const ColumnCollection& cc,
                     OutputWrapper<int>& ans,
                     const BulkBoolOp bulk_op,
                     const SEXP& comp_op,
                     const Rcpp::List& target_val);

    virtual void work_row(std::size_t in_id, std::size_t out_id) override;

private:
    const NAVisitor na_visitor_;

    const NaAction na_action_;
    OutputWrapper<int>& ans_;
    const BulkBoolOp bulk_op_;
    const LogicalOperator op_;
    const CompOp comp_op_;
    std::vector<std::shared_ptr<BooleanVisitor>> visitors_;
    std::vector<bool> na_targets_;

    // sigh, for case TRUE == "TRUE"
    std::vector<char *> char_targets_;
    const ComparisonOperator comp_operator_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANWORKERS_H_
