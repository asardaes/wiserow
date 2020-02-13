#include "integer-workers.h"

#include <stdexcept> // logic_error
#include <string>

#include <boost/variant/get.hpp>

namespace wiserow {

InSetWorker::InSetWorker(const OperationMetadata& metadata,
                         const ColumnCollection& cc,
                         OutputWrapper<int>& ans,
                         const Rcpp::List& target_sets,
                         const bool negate,
                         const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , out_strategy_(out_strategy)
{
    if (!out_strategy_) { // nocov start
        throw std::logic_error("Output strategy cannot be null.");
    } // nocov end

    for (R_xlen_t i = 0; i < target_sets.length(); i++) {
        visitors_.push_back(BooleanVisitorBuilder().in_set(target_sets[i], negate).build());
        char_targets_.push_back(TYPEOF(target_sets[i]) == STRSXP);
    }
}

ParallelWorker::thread_local_ptr InSetWorker::work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) {
    std::shared_ptr<OutputStrategy<int>> thread_local_strategy =
        t_local ? std::static_pointer_cast<OutputStrategy<int>>(t_local) : out_strategy_->clone();

    thread_local_strategy->reinit();

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        auto visitor = visitors_[j % visitors_.size()];
        supported_col_t variant = col_collection_(in_id, j);

        if (char_targets_[j % char_targets_.size()] && col_collection_[j]->is_logical()) {
            // tricky case when source is R-logical (with underlying int) that should be converted to string
            int variant_int = boost::get<int>(variant);
            if (variant_int == NA_INTEGER) {
                thread_local_strategy->apply(j, variant, boost::apply_visitor(*visitor, variant));
            }
            else {
                const char *int_char = variant_int == 0 ? "FALSE" : "TRUE";
                boost::string_ref str_ref(int_char);
                thread_local_strategy->apply(j, variant, (*visitor)(str_ref));
            }
        }
        else {
            thread_local_strategy->apply(j, variant, boost::apply_visitor(*visitor, variant));
        }

        if (thread_local_strategy->short_circuit()) {
            break;
        }
    }

    ans_[out_id] = thread_local_strategy->output(metadata, col_collection_.ncol(), false);
    return thread_local_strategy;
}

} // namespace wiserow
