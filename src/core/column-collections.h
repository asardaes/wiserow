#ifndef WISEROW_COLUMN_COLLECTIONS_H_
#define WISEROW_COLUMN_COLLECTIONS_H_

#include <cstddef> // std::size_t
#include <memory> // std::shared_ptr
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

class IntegerMatrixColumnCollection : public ColumnCollection
{
public:
    IntegerMatrixColumnCollection(const Rcpp::IntegerMatrix& mat);
};

} // namespace wiserow

#endif // WISEROW_COLUMN_COLLECTIONS_H_
