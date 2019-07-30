#ifndef WISEROW_VISITORS_H_
#define WISEROW_VISITORS_H_

#include <cstddef> // std::size_t

#include <boost/variant.hpp>

#include "../core/OperationMetadata.h"
#include "../core/ParallelWorker.h"
#include "../core/columns.h"

namespace wiserow {

// =================================================================================================

template<typename T>
class SumVisitor : public ParallelWorker, public boost::static_visitor<T>
{
public:
    SumVisitor(const OperationMetadata& metadata, const ColumnCollection& cc, T * const ans)
        : ParallelWorker(cc.nrow() / metadata.num_workers, 1000, 10000)
        , col_collection_(cc)
        , ans_(ans)
    { }

    template<typename U>
    T operator()(const U val) const {
        return val;
    }

    void work_it(std::size_t begin, std::size_t end) override {
        for (std::size_t i = begin; i < end; i++) {
            if (is_interrupted(i)) break; // nocov

            ans_[i] = T(0);

            for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
                ans_[i] += boost::apply_visitor(*this, col_collection_(i,j));
            }
        }
    }

private:
    const ColumnCollection col_collection_;
    T * const ans_;
};

} // namespace wiserow

#endif // WISEROW_VISITORS_H_
