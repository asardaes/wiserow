#ifndef WISEROW_MATRIXCOLUMNCOLLECTION_H_
#define WISEROW_MATRIXCOLUMNCOLLECTION_H_

#include <cstddef> // size_t
#include <memory> // make_shared

#include <Rcpp.h>

#include "SurrogateColumn.h"
#include "columns.h"

namespace wiserow {

template<int RT, typename T>
class MatrixColumnCollection : public ColumnCollection
{
public:
    MatrixColumnCollection(const Rcpp::Matrix<RT>& mat, const surrogate_vector& cols)
        : ColumnCollection(mat.nrow())
    {
        if (cols.ptr) {
            for (std::size_t i = 0; i < cols.len; i++) {
                int j = cols.ptr[i] - 1;
                columns_.push_back(std::make_shared<SurrogateColumn<T>>(&mat[j * nrow_], nrow_));
            }
        }
        else if (cols.is_null) {
            for (int j = 0; j < mat.ncol(); j++) {
                columns_.push_back(std::make_shared<SurrogateColumn<T>>(&mat[j * nrow_], nrow_));
            }
        }
    }
};

// -------------------------------------------------------------------------------------------------
// Specialization for STRSXP
template<>
class MatrixColumnCollection<STRSXP, const char *> : public ColumnCollection
{
public:
    MatrixColumnCollection(const Rcpp::Matrix<STRSXP>& mat, const surrogate_vector& cols);
};

// -------------------------------------------------------------------------------------------------
// Specialization for CPLXSXP
template<>
class MatrixColumnCollection<CPLXSXP, std::complex<double>> : public ColumnCollection
{
public:
    MatrixColumnCollection(const Rcpp::Matrix<CPLXSXP>& mat, const surrogate_vector& cols);
};

} // namespace wiserow

#endif // WISEROW_MATRIXCOLUMNCOLLECTION_H_
