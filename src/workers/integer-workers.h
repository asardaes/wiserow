#ifndef WISEROW_INTEGERWORKERS_H_
#define WISEROW_INTEGERWORKERS_H_

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

class CompBasedWorker : public ParallelWorker
{
public:
    CompBasedWorker(const OperationMetadata& metadata,
                    const ColumnCollection& cc,
                    OutputWrapper<int>& ans,
                    const SEXP& comp_op,
                    const Rcpp::List& target_vals,
                    const std::shared_ptr<OutputStrategy<int>>& out_strategy);

    virtual thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) override;

private:
    const NAVisitor na_visitor_;

    OutputWrapper<int>& ans_;
    const CompOp comp_op_;
    const std::shared_ptr<OutputStrategy<int>> out_strategy_;

    std::vector<std::shared_ptr<BooleanVisitor>> visitors_;
    std::vector<bool> na_targets_;

    // sigh, for case TRUE == "TRUE"
    std::vector<char *> char_targets_;
    const ComparisonOperator comp_operator_;
};

} // namespace wiserow

#endif // WISEROW_INTEGERWORKERS_H_
