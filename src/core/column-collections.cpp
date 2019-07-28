#include "column-collections.h"

namespace wiserow {

ColumnCollection::ColumnCollection(const std::size_t nrow)
    : nrow_(nrow)
{ }

std::size_t ColumnCollection::ncol() const {
    return columns_.size();
}

std::size_t ColumnCollection::nrow() const {
    return nrow_;
}

const VariantColumn& ColumnCollection::operator[](const std::size_t j) {
    return *(columns_[j]);
}

IntegerMatrixColumnCollection::IntegerMatrixColumnCollection(const Rcpp::IntegerMatrix& mat)
    : ColumnCollection(mat.nrow())
{
    for (int j = 0; j < mat.ncol(); j++) {
        columns_.push_back(
            std::make_shared<SurrogateColumn<int>>(&mat[static_cast<std::size_t>(j) * nrow_], nrow_)
        );
    }
}

} // namespace wiserow
