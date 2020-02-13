#include "integer-workers.h"

#include <boost/variant/get.hpp>

namespace wiserow {

DuplicatedWorker::DuplicatedWorker(const OperationMetadata& metadata,
                                   const ColumnCollection& cc,
                                   OutputWrapper<int>& ans,
                                   const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , out_strategy_(out_strategy)
{ }

ParallelWorker::thread_local_ptr DuplicatedWorker::work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) {
    std::shared_ptr<OutputStrategy<int>> thread_local_strategy =
        t_local ? std::static_pointer_cast<OutputStrategy<int>>(t_local) : out_strategy_->clone();

    DuplicatedVisitor duplicated_visitor;

    if (thread_local_strategy) {
        thread_local_strategy->reinit();

        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            supported_col_t variant = col_collection_(in_id, j);

            if (col_collection_[j]->is_logical()) {
                int variant_int = boost::get<int>(variant);

                if (variant_int == NA_INTEGER) {
                    thread_local_strategy->apply(j, variant, boost::apply_visitor(duplicated_visitor, variant));
                }
                else {
                    bool int_bool = static_cast<bool>(variant_int);
                    thread_local_strategy->apply(j, variant, (duplicated_visitor)(int_bool));
                }
            }
            else {
                thread_local_strategy->apply(j, variant, boost::apply_visitor(duplicated_visitor, variant));
            }

            if (thread_local_strategy->short_circuit()) {
                break;
            }
        }

        ans_[out_id] = thread_local_strategy->output(metadata, col_collection_.ncol(), false);
        return thread_local_strategy;
    }
    else {
        // thread_local_strategy is null -> IdentityStrategy
        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            supported_col_t variant = col_collection_(in_id, j);

            if (col_collection_[j]->is_logical()) {
                int variant_int = boost::get<int>(variant);

                if (variant_int == NA_INTEGER) {
                    ans_(out_id, j) = boost::apply_visitor(duplicated_visitor, variant);
                }
                else {
                    bool int_bool = static_cast<bool>(variant_int);
                    ans_(out_id, j) = (duplicated_visitor)(int_bool);
                }
            }
            else {
                ans_(out_id, j) = boost::apply_visitor(duplicated_visitor, variant);
            }
        }

        return nullptr;
    }
}

} // namespace wiserow
