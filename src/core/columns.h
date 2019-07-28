#ifndef WISEROW_COLUMNS_H_
#define WISEROW_COLUMNS_H_

#include <cstddef> // std::size_t
#include <stdexcept>
#include <string>

#include "types.h"

namespace wiserow {

// =================================================================================================

class VariantColumn
{
public:
    virtual ~VariantColumn() {}
    virtual const supported_col_t operator[](const std::size_t id) const = 0;
};

// =================================================================================================

template<typename T>
class SurrogateColumn : public VariantColumn
{
public:
    SurrogateColumn(T const * const data_ptr, const std::size_t size)
        : data_ptr_(data_ptr)
        , size_(size)
    { }

    const supported_col_t operator[](const std::size_t id) const override {
        if (id >= size_) {
            throw std::out_of_range("Column of size " +
                                    std::to_string(size_) +
                                    " cannot be indexed at " +
                                    std::to_string(id));
        }

        return supported_col_t(data_ptr_[id]);
    }

private:
    T const * const data_ptr_;
    const std::size_t size_;
};

} // namespace wiserow

#endif // WISEROW_COLUMNS_H_
