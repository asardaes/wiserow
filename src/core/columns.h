#ifndef WISEROW_COLUMNS_H_
#define WISEROW_COLUMNS_H_

#include <cstddef> // size_t
#include <memory> // shared_ptr
#include <stdexcept> // out_of_range
#include <type_traits> // conditional, is_same
#include <vector>

#include <Rcpp.h>
#include <boost/variant/variant.hpp>

#include "OperationMetadata.h"

namespace wiserow {

typedef boost::variant<int, double> supported_col_t;

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
        if (id >= size_) { // nocov start
            throw std::out_of_range("Column of size " +
                                    std::to_string(size_) +
                                    " cannot be indexed at " +
                                    std::to_string(id));
        } // nocov end

        return supported_col_t(data_ptr_[id]);
    }

private:
    T const * const data_ptr_;
    const std::size_t size_;
};

// =================================================================================================

class ColumnCollection
{
public:
    static ColumnCollection coerce(const OperationMetadata& metadata, SEXP data);

    ColumnCollection(const std::size_t nrow);
    virtual ~ColumnCollection() {}

    std::size_t nrow() const;
    std::size_t ncol() const;

    const VariantColumn& operator[](const std::size_t j) const;
    const supported_col_t operator()(const std::size_t i, const std::size_t j) const;

protected:
    std::vector<std::shared_ptr<const VariantColumn>> columns_;
    const std::size_t nrow_;
};

// =================================================================================================

template<typename T>
class MatrixColumnCollection : public ColumnCollection
{
public:
    MatrixColumnCollection(const T& mat)
        : ColumnCollection(mat.nrow())
    {
        typedef typename std::conditional<std::is_same<T, Rcpp::IntegerMatrix>::value, int, double>::type U;

        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(
                std::make_shared<SurrogateColumn<U>>(&mat[static_cast<std::size_t>(j) * nrow_], nrow_)
            );
        }
    }
};

} // namespace wiserow

#endif // WISEROW_COLUMNS_H_
