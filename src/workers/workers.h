#ifndef WISEROW_WORKERS_H_
#define WISEROW_WORKERS_H_

#include <cstddef> // size_t
#include <memory> // shared_ptr
#include <type_traits> // is_same

#include <boost/variant.hpp>

#include "../core/OperationMetadata.h"
#include "../core/OutputWrapper.h"
#include "../core/ParallelWorker.h"
#include "../core/columns.h"
#include "../visitors/visitors.h"

namespace wiserow {

// =================================================================================================

template<typename T>
class RowSumsWorker : public ParallelWorker
{
public:
    RowSumsWorker(const OperationMetadata& metadata, const ColumnCollection& cc, std::shared_ptr<OutputWrapper<T>> ans)
        : ParallelWorker(metadata, cc)
        , ans_(ans)
    { }

    void work_row(std::size_t in_id, std::size_t out_id) override {
        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            bool is_na = boost::apply_visitor(na_visitor_, col_collection_(in_id, j));

            if (is_na) {
                if (metadata.na_action == NaAction::pass) {
                    (*ans_)[out_id] = na_value_;
                    break;
                }
            }
            else {
                (*ans_)[out_id] += boost::apply_visitor(visitor_, col_collection_(in_id, j));
            }
        }
    }

private:
    std::shared_ptr<OutputWrapper<T>> ans_;

    const T na_value_ = std::is_same<T, int>::value ? NA_INTEGER : NA_REAL;
    const NAVisitor na_visitor_;
    const NumericVisitor<T> visitor_;
};

} // namespace wiserow

#endif // WISEROW_WORKERS_H_
