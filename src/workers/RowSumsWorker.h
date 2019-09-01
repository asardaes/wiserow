#ifndef WISEROW_ROWSUMSWORKER_H_
#define WISEROW_ROWSUMSWORKER_H_

#include <cstddef> // size_t

#include <boost/variant.hpp>
#include <Rcpp.h>

#include "../core.h"
#include "../utils.h"
#include "../visitors/boolean-visitors.h"

namespace wiserow {

template<typename T>
class RowSumsWorker : public ParallelWorker
{
public:
    RowSumsWorker(const OperationMetadata& metadata, const ColumnCollection& cc, OutputWrapper<T>& ans)
        : ParallelWorker(metadata, cc)
        , ans_(ans)
    { }

    thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr) override {
        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            bool is_na = boost::apply_visitor(na_visitor_, col_collection_(in_id, j));

            if (is_na) {
                if (metadata.na_action == NaAction::PASS) {
                    ans_[out_id] = na_value_;
                    break;
                }
            }
            else {
                ans_[out_id] += boost::apply_visitor(visitor_, col_collection_(in_id, j));
            }
        }

        return nullptr;
    }

private:
    OutputWrapper<T>& ans_;

    const T na_value_ = NA_REAL;
    const NAVisitor na_visitor_;
    const NumericVisitor<T> visitor_;
};

// -------------------------------------------------------------------------------------------------
// Specialization for logical (which is integer in R) because any int > 1 is not really TRUE for R

template<>
class RowSumsWorker<int> : public ParallelWorker
{
public:
    RowSumsWorker(const OperationMetadata& metadata, const ColumnCollection& cc, OutputWrapper<int>& ans)
        : ParallelWorker(metadata, cc)
        , ans_(ans)
    { }

    thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr) override {
        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            bool is_na = boost::apply_visitor(na_visitor_, col_collection_(in_id, j));

            if (is_na) {
                if (metadata.na_action == NaAction::PASS) {
                    ans_[out_id] = na_value_;
                    break;
                }
            }
            else {
                ans_[out_id] += boost::apply_visitor(visitor_, col_collection_(in_id, j));
            }
        }

        int ans = ans_[out_id];
        if (metadata.output_mode == LGLSXP && ans != na_value_ && ans > 1) {
            ans_[out_id] = 1;
        }

        return nullptr;
    }

private:
    OutputWrapper<int>& ans_;

    const int na_value_ = NA_INTEGER;
    const NAVisitor na_visitor_;
    const NumericVisitor<int> visitor_;
};

} // namespace wiserow

#endif // WISEROW_ROWSUMSWORKER_H_
