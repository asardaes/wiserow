#ifndef WISEROW_COLUMN_COLLECTIONS_H_
#define WISEROW_COLUMN_COLLECTIONS_H_

#include <cstddef> // std::size_t
#include <memory> // std::shared_ptr
#include <type_traits> // conditional, is_same
#include <vector>

#include <Rcpp.h>

#include "columns.h"

namespace wiserow {

// =================================================================================================

class ColumnCollection
{
public:
    ColumnCollection(const std::size_t nrow);
    virtual ~ColumnCollection() {}

    std::size_t nrow() const;
    std::size_t ncol() const;
    const VariantColumn& operator[](const std::size_t j);

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
        typedef typename std::conditional<std::is_same<T, Rcpp::IntegerMatrix>::value, int, double>::type OUT;

        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(
                std::make_shared<SurrogateColumn<OUT>>(&mat[static_cast<std::size_t>(j) * nrow_], nrow_)
            );
        }
    }
};

} // namespace wiserow

#endif // WISEROW_COLUMN_COLLECTIONS_H_
