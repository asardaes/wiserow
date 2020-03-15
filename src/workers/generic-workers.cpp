#include "generic-workers.h"

namespace wiserow {

boost::string_ref RowExtremaWorker<boost::string_ref, false>::STRING_REF_NOT_SET = boost::string_ref("dummy");

// -------------------------------------------------------------------------------------------------

RowExtremaWorker<boost::string_ref, false>::RowExtremaWorker(const OperationMetadata& metadata,
                                                             const ColumnCollection& cc,
                                                             const Rcpp::List extras)
    : ParallelWorker(metadata, cc)
    , ans(cc.nrow())
    , comp_op_(parse_comp_op(Rcpp::as<std::string>(extras["comp_op"])))
    , dummy_parent_visitor_(std::make_shared<InitBooleanVisitor>(true))
{ }

// -------------------------------------------------------------------------------------------------

ParallelWorker::thread_local_ptr RowExtremaWorker<boost::string_ref, false>::work_row(std::size_t in_id,
                                                                                      std::size_t out_id,
                                                                                      ParallelWorker::thread_local_ptr t_local)
{
    supported_col_t variant;
    bool variant_initialized = false;
    std::shared_ptr<BooleanVisitor> visitor = nullptr;

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        const supported_col_t& next_variant = col_collection_(in_id, j);
        bool is_na = boost::apply_visitor(na_visitor_, next_variant);

        if (is_na) {
            if (metadata.na_action == NaAction::EXCLUDE) {
                continue;
            }
            else {
                variant = STRING_REF_NOT_SET;
                variant_initialized = true;
                break;
            }
        }
        else if (!visitor) {
            variant = coerce(next_variant, col_collection_[j]->is_logical());
            visitor = instantiate_visitor(boost::get<boost::string_ref>(variant));
        }
        else {
            const supported_col_t coerced_next = coerce(next_variant, col_collection_[j]->is_logical());
            bool next_more_extreme = boost::apply_visitor(*visitor, coerced_next);
            if (next_more_extreme) {
                variant = coerced_next;
                visitor = instantiate_visitor(boost::get<boost::string_ref>(variant));
            }
        }

        variant_initialized = true;
    }

    ans[out_id] = variant_initialized ? boost::get<boost::string_ref>(variant) : STRING_REF_NOT_SET;
    return nullptr;
}

// -------------------------------------------------------------------------------------------------

std::shared_ptr<BooleanVisitor> RowExtremaWorker<boost::string_ref, false>::instantiate_visitor(const boost::string_ref& str_ref) {
    return std::make_shared<ComparisonVisitor<boost::string_ref>>(bool_op_,
                                                                  comp_op_,
                                                                  str_ref,
                                                                  dummy_parent_visitor_);
}

// -------------------------------------------------------------------------------------------------

boost::string_ref RowExtremaWorker<boost::string_ref, false>::coerce(const supported_col_t& variant, const bool is_logical) {
    std::string val;

    if (variant.type() == typeid(int)) {
        val = is_logical ? ::wiserow::to_string(boost::get<int>(variant) != 0) :  ::wiserow::to_string(boost::get<int>(variant));
    }
    else if (variant.type() == typeid(double)) {
        val = ::wiserow::to_string(boost::get<double>(variant));
    }
    else if (variant.type() == typeid(boost::string_ref)) {
        return boost::get<boost::string_ref>(variant);
    }
    else {
        throw std::runtime_error("[wiserow] Invalid type passed to RowExtremaWorker. This should not happen."); // nocov
    }

    this->mutex_.lock();
    auto it = temporary_strings_.insert(std::move(val));
    this->mutex_.unlock();
    return boost::string_ref(it.first->c_str());
}

} // namespace wiserow
