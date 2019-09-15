#ifndef WISEROW_BOOLEANWORKERS_H_
#define WISEROW_BOOLEANWORKERS_H_

#include <cstddef> // size_t
#include <memory>
#include <vector>

#include <Rcpp.h>

#include "../core.h"
#include "../utils.h"
#include "../visitors.h"
#include "worker-strategies.h"

namespace wiserow {

// =================================================================================================

class BoolTestWorker : public ParallelWorker
{
public:
    BoolTestWorker(const OperationMetadata& metadata,
                   const ColumnCollection& cc,
                   OutputWrapper<int>& ans,
                   const std::shared_ptr<BooleanVisitor>& visitor,
                   const std::shared_ptr<OutputStrategy<int>>& out_strategy);

    virtual thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) override;

private:
    OutputWrapper<int>& ans_;
    const std::shared_ptr<BooleanVisitor> visitor_;
    const std::shared_ptr<OutputStrategy<int>> out_strategy_;
};

// =================================================================================================

class NATestWorker : public BoolTestWorker
{
public:
    NATestWorker(const OperationMetadata& metadata,
                 const ColumnCollection& cc,
                 OutputWrapper<int>& ans,
                 const std::shared_ptr<OutputStrategy<int>>& out_strategy);
};

// =================================================================================================

class InfTestWorker : public BoolTestWorker
{
public:
    InfTestWorker(const OperationMetadata& metadata,
                  const ColumnCollection& cc,
                  OutputWrapper<int>& ans,
                  const std::shared_ptr<OutputStrategy<int>>& out_strategy);
};

// =================================================================================================

class FiniteTestWorker : public BoolTestWorker
{
public:
    FiniteTestWorker(const OperationMetadata& metadata,
                     const ColumnCollection& cc,
                     OutputWrapper<int>& ans,
                     const std::shared_ptr<OutputStrategy<int>>& out_strategy);
};

} // namespace wiserow

#endif // WISEROW_BOOLEANWORKERS_H_
