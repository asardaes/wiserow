#include "OutputWrapper.h"

namespace wiserow {

VectorOutputWrapper<CPLXSXP, std::complex<double>>::VectorOutputWrapper(Rcpp::ComplexVector vec)
    : data_(reinterpret_cast<std::complex<double> *>(&vec[0]))
    , len_(vec.length())
{ }

// -------------------------------------------------------------------------------------------------

std::complex<double>& VectorOutputWrapper<CPLXSXP, std::complex<double>>::operator()(const std::size_t i,
                                                                                   const std::size_t j)
{
    // nocov start
    if (j > 0) {
        throw std::out_of_range("[wiserow] attempted to index a vector of length " +
                                std::to_string(len_) +
                                " as matrix at column " +
                                std::to_string(j));
    }

    if (i >= len_) {
        throw std::out_of_range("[wiserow] attempted to index a vector of length " +
                                std::to_string(len_) +
                                " at " +
                                std::to_string(i + 1));
    }
    // nocov end

    return data_[i];
}

// =================================================================================================

ListOutputWrapper<CPLXSXP, std::complex<double>>::ListOutputWrapper(Rcpp::List data)
    : data_(data.length())
    , lens_(data.length())
{
    for (R_xlen_t i = 0; i < data.length(); i++) {
        Rcpp::ComplexVector one_vec(data[i]);
        lens_[i] = one_vec.length();
        if (one_vec.length() > 0) {
            data_[i] = reinterpret_cast<std::complex<double> *>(&one_vec[0]);
        }
    }
}

// -------------------------------------------------------------------------------------------------

std::complex<double>& ListOutputWrapper<CPLXSXP, std::complex<double>>::operator()(const std::size_t i,
                                                                                 const std::size_t j)
{
    // nocov start
    if (i >= data_.size()) {
        throw std::out_of_range("[wiserow] attempted to index a list of length " +
                                std::to_string(data_.size()) +
                                " at " +
                                std::to_string(i + 1));
    }

    if (j >= lens_[i]) {
        throw std::out_of_range("[wiserow] attempted to index an enlisted vector of length " +
                                std::to_string(lens_[i]) +
                                " at " +
                                std::to_string(j + 1));
    }
    // nocov end

    return data_[i][j];
}

// =================================================================================================

DataFrameOutputWrapper<CPLXSXP, std::complex<double>>::DataFrameOutputWrapper(Rcpp::DataFrame data)
    : cols_(data.ncol())
    , lens_(data.ncol())
{
    for (R_xlen_t j = 0; j < data.ncol(); j++) {
        Rcpp::Vector<CPLXSXP> one_vec(data[j]);
        lens_[j] = one_vec.length();
        if (one_vec.length() > 0) {
            cols_[j] = reinterpret_cast<std::complex<double> *>(&one_vec[0]);
        }
    }
}

// -------------------------------------------------------------------------------------------------

std::complex<double>& DataFrameOutputWrapper<CPLXSXP, std::complex<double>>::operator()(const std::size_t i,
                                                                                      const std::size_t j)
{
    // nocov start
    if (j >= cols_.size()) {
        throw std::out_of_range("[wiserow] attempted to index data frame with " +
                                std::to_string(cols_.size()) +
                                " columns at column " +
                                std::to_string(j + 1));
    }

    if (i >= lens_[j]) {
        throw std::out_of_range("[wiserow] attempted to index a data frame with " +
                                std::to_string(lens_[j]) +
                                " rows at row " +
                                std::to_string(i + 1));
    }
    // nocov end

    return cols_[j][i];
}

// =================================================================================================

MatrixOutputWrapper<CPLXSXP, std::complex<double>>::MatrixOutputWrapper(Rcpp::Matrix<CPLXSXP> data)
    : ncol_(data.ncol())
    , nrow_(data.nrow())
    , data_(reinterpret_cast<std::complex<double> *>(&data[0]))
{
    // nocov start
    if (ncol_ == 0 || nrow_ == 0) {
        throw std::invalid_argument("[wiserow] output matrix is empty.");
    }
    // nocov end
}

// -------------------------------------------------------------------------------------------------

std::complex<double>& MatrixOutputWrapper<CPLXSXP, std::complex<double>>::operator()(const std::size_t i,
                                                                                   const std::size_t j)
{
    // nocov start
    if (j >= ncol_) {
        throw std::out_of_range("[wiserow] attempted to index matrix with " +
                                std::to_string(ncol_) +
                                " columns at column " +
                                std::to_string(j + 1));
    }

    if (i >= nrow_) {
        throw std::out_of_range("[wiserow] attempted to index a matrix with " +
                                std::to_string(nrow_) +
                                " rows at row " +
                                std::to_string(i + 1));
    }
    // nocov end

    return data_[i + j * nrow_];
}

} // namespace wiserow
