#include "integer-workers.h"

namespace wiserow {

BoolTestWorker::BoolTestWorker(const OperationMetadata& metadata,
                               const ColumnCollection& cc,
                               OutputWrapper<int>& ans,
                               const std::shared_ptr<BooleanVisitor>& visitor,
                               const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , visitor_(visitor)
    , out_strategy_(out_strategy)
{ }

// -------------------------------------------------------------------------------------------------

ParallelWorker::thread_local_ptr BoolTestWorker::work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) {
    std::shared_ptr<OutputStrategy<int>> thread_local_strategy =
            t_local ? std::static_pointer_cast<OutputStrategy<int>>(t_local) : out_strategy_->clone();

    thread_local_strategy->reinit();

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        supported_col_t variant = col_collection_(in_id, j);
        thread_local_strategy->apply(j, variant, boost::apply_visitor(*visitor_, variant));

        if (thread_local_strategy->short_circuit()) {
            break;
        }
    }

    ans_[out_id] = thread_local_strategy->output(metadata, col_collection_.ncol(), false);
    return thread_local_strategy;
}

// =================================================================================================

NATestWorker::NATestWorker(const OperationMetadata& metadata,
                           const ColumnCollection& cc,
                           OutputWrapper<int>& ans,
                           const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : BoolTestWorker(metadata, cc, ans, BooleanVisitorBuilder().is_na().build(), out_strategy)
{ }

// =================================================================================================

InfTestWorker::InfTestWorker(const OperationMetadata& metadata,
                             const ColumnCollection& cc,
                             OutputWrapper<int>& ans,
                             const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : BoolTestWorker(metadata, cc, ans, BooleanVisitorBuilder().is_inf().build(), out_strategy)
{ }

// =================================================================================================

FiniteTestWorker::FiniteTestWorker(const OperationMetadata& metadata,
                                   const ColumnCollection& cc,
                                   OutputWrapper<int>& ans,
                                   const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : BoolTestWorker(metadata, cc, ans, BooleanVisitorBuilder(BoolOp::AND, true).is_na(true).is_inf(true).build(), out_strategy)
{ }

} // namespace wiserow
