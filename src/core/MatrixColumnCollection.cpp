#include "MatrixColumnCollection.h"

namespace wiserow {

MatrixColumnCollection<STRSXP, const char *>::MatrixColumnCollection(const Rcpp::Matrix<STRSXP>& mat,
                                                                     const surrogate_vector& cols)
    : ColumnCollection(mat.nrow())
{
    if (cols.ptr) {
        for (std::size_t i = 0; i < cols.len; i++) {
            int j = cols.ptr[i] - 1;
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringMatrix>>(mat, j));
        }
    }
    else if (cols.is_null) {
        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringMatrix>>(mat, j));
        }
    }
}

// -------------------------------------------------------------------------------------------------

MatrixColumnCollection<CPLXSXP, std::complex<double>>::MatrixColumnCollection(const Rcpp::Matrix<CPLXSXP>& mat,
                                                                              const surrogate_vector& cols)
    : ColumnCollection(mat.nrow())
{
    if (cols.ptr) {
        for (std::size_t i = 0; i < cols.len; i++) {
            int j = cols.ptr[i] - 1;
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexMatrix>>(mat, j));
        }
    }
    else if (cols.is_null) {
        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexMatrix>>(mat, j));
        }
    }
}

} // namespace wiserow
