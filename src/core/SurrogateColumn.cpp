#include "MatrixColumnCollection.h"

#include <stdexcept> // out_of_range

namespace wiserow {

/*
 * Accessing elements of STRSXP is based on STRING_ELT:
 * https://github.com/wch/r-source/blob/aa024e5fa456871f9825e9f257ae6872a9cb0722/src/main/memory.c#L3750
 * which moved to https://github.com/wch/r-source/blob/76c648e4bd0828f5195e88659d484c7e9c1d6204/src/main/memory.c#L4102
 *
 * STRING_ELT calls STDVEC_DATAPTR and casts the result to SEXP*:
 * https://github.com/wch/r-source/blob/aa024e5fa456871f9825e9f257ae6872a9cb0722/src/include/Rinternals.h#L441
 * which moved to https://github.com/wch/r-source/blob/76c648e4bd0828f5195e88659d484c7e9c1d6204/src/include/Defn.h#L413
 *
 * To get the final char* from SEXP*, we use CHAR() again. This, along with STDVEC_DATAPTR, are only casts
 * with some arithmetic, so they should be thread safe (?).
 */

SurrogateColumn<Rcpp::StringMatrix>::SurrogateColumn(SEXP mat, const int j)
    : data_(mat)
    , size_(Rf_nrows(mat))
    , offset_(j * size_)
{ }

// -------------------------------------------------------------------------------------------------

const supported_col_t SurrogateColumn<Rcpp::StringMatrix>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    SEXP element = STRING_ELT(data_, id + offset_);
    return supported_col_t(boost::string_ref(CHAR(element)));
}

// =================================================================================================

SurrogateColumn<Rcpp::StringVector>::SurrogateColumn(SEXP vec)
    : data_(vec)
    , size_(Rf_xlength(vec))
{ }

// -------------------------------------------------------------------------------------------------

const supported_col_t SurrogateColumn<Rcpp::StringVector>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    SEXP element = STRING_ELT(data_, id);
    return supported_col_t(boost::string_ref(CHAR(element)));
}

// =================================================================================================

SurrogateColumn<Rcpp::ComplexMatrix>::SurrogateColumn(const Rcpp::ComplexMatrix& mat, const int j)
    : data_ptr_(reinterpret_cast<const std::complex<double> *>(&mat[j * mat.nrow()]))
    , size_(mat.nrow())
{ }

// -------------------------------------------------------------------------------------------------

const supported_col_t SurrogateColumn<Rcpp::ComplexMatrix>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(data_ptr_[id]);
}

// =================================================================================================

SurrogateColumn<Rcpp::ComplexVector>::SurrogateColumn(const Rcpp::ComplexVector& vec)
    : data_ptr_(reinterpret_cast<const std::complex<double> *>(&vec[0]))
    , size_(vec.length())
{ }

// -------------------------------------------------------------------------------------------------

const supported_col_t SurrogateColumn<Rcpp::ComplexVector>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(data_ptr_[id]);
}

} // namespace wiserow
