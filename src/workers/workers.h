#ifndef WISEROW_WORKERS_H_
#define WISEROW_WORKERS_H_

#include <cstddef> // size_t
#include <type_traits> // is_same

#include <boost/variant.hpp>

#include "../core/OperationMetadata.h"
#include "../core/ParallelWorker.h"
#include "../core/columns.h"
#include "../visitors/visitors.h"

namespace wiserow {

// =================================================================================================

template<typename T>
class RowSumsWorker : public ParallelWorker
{
public:
    RowSumsWorker(const OperationMetadata& metadata, const ColumnCollection& cc, T * const ans)
        : ParallelWorker(metadata, cc.nrow() / metadata.num_workers, 1000, 10000)
        , input_mode_(metadata.input_modes[0])
        , col_collection_(cc)
        , ans_(ans)
    { }

    void work_it(std::size_t begin, std::size_t end) override {
        for (std::size_t i = begin; i < end; i++) {
            if (eptr || is_interrupted(i)) break; // nocov

            for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
                if (boost::apply_visitor(na_visitor_, col_collection_(i,j))) {
                    if (metadata_.na_action == "pass") {
                        ans_[i] = na_value_;
                        break;
                    }
                }
                else {
                    ans_[i] += boost::apply_visitor(visitor_, col_collection_(i,j));
                }
            }
        }
    }

private:
    const int input_mode_;
    const ColumnCollection col_collection_;
    T * const ans_;

    const NAVisitor na_visitor_;
    const NumericVisitor<T> visitor_;
    const T na_value_ = std::is_same<T, int>::value ? NA_INTEGER : NA_REAL;
};

} // namespace wiserow

#endif // WISEROW_WORKERS_H_
