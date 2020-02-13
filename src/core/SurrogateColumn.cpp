#include "MatrixColumnCollection.h"

#include <stdexcept> // out_of_range

namespace wiserow {

/*
 * Accessing elements of STRSXP is based on STRING_ELT:
 * https://github.com/wch/r-source/blob/aa024e5fa456871f9825e9f257ae6872a9cb0722/src/main/memory.c#L3750
 *
 * STRING_ELT calls STDVEC_DATAPTR and casts the result to SEXP*:
 * https://github.com/wch/r-source/blob/aa024e5fa456871f9825e9f257ae6872a9cb0722/src/include/Rinternals.h#L441
 *
 * To get the final char* from SEXP*, we use CHAR(). This, along with STDVEC_DATAPTR, are only casts
 * with some arithmetic, so they should be thread safe (?).
 */

SurrogateColumn<Rcpp::StringMatrix>::SurrogateColumn(SEXP mat, const int j)
    : data_ptr_(static_cast<SEXP *>(STDVEC_DATAPTR(mat)))
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

    return supported_col_t(boost::string_ref(CHAR(data_ptr_[id + offset_])));
}

// =================================================================================================

SurrogateColumn<Rcpp::StringVector>::SurrogateColumn(SEXP vec)
    : data_ptr_(static_cast<SEXP *>(STDVEC_DATAPTR(vec)))
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

    return supported_col_t(boost::string_ref(CHAR(data_ptr_[id])));
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
