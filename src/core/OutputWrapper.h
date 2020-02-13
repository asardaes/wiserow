/**
 * The constructors here take by value because we're mostly dealing with wrappers, so copies are
 * cheap, and it makes it easier to instantiate shared_ptr<OutputWrapper<T>> by passing a SEXP
 * variant to the constructors.
 */

#ifndef WISEROW_OUTPUTWRAPPER_H_
#define WISEROW_OUTPUTWRAPPER_H_

#include <complex>
#include <cstddef> // size_t
#include <stdexcept> // out_of_range
#include <string>
#include <vector>

#include <Rcpp.h>

#include "OperationMetadata.h"

namespace wiserow {

template<typename T>
class OutputWrapper {
public:
    virtual ~OutputWrapper() {} // nocov

    virtual T& operator[](const std::size_t i) {
        return (*this)(i, 0);
    }

    virtual T& operator()(const std::size_t i, const std::size_t j) = 0;
};

// =================================================================================================

template<R_vec_t RT, typename T>
class VectorOutputWrapper : public OutputWrapper<T> {
public:
    VectorOutputWrapper(Rcpp::Vector<RT> data)
        : data_(&data[0])
        , len_(data.length())
    { }

    virtual T& operator()(const std::size_t i, const std::size_t j) override {
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

private:
    T * const data_;
    const std::size_t len_;
};

// -------------------------------------------------------------------------------------------------
// Specialization for CPLXSXP, std::complex<double>

template<>
class VectorOutputWrapper<CPLXSXP, std::complex<double>> : public OutputWrapper<std::complex<double>> {
public:
    VectorOutputWrapper(Rcpp::ComplexVector vec);

    virtual std::complex<double>& operator()(const std::size_t i, const std::size_t j) override;

private:
    std::complex<double> * const data_;
    const std::size_t len_;
};

// =================================================================================================

template<R_vec_t RT, typename T>
class ListOutputWrapper : public OutputWrapper<T> {
public:
    ListOutputWrapper(Rcpp::List data)
        : data_(data.length())
        , lens_(data.length())
    {
        for (R_xlen_t i = 0; i < data.length(); i++) {
            Rcpp::Vector<RT> one_vec(data[i]);
            lens_[i] = one_vec.length();
            if (one_vec.length() > 0) {
                data_[i] = &one_vec[0];
            }
        }
    }

    virtual T& operator()(const std::size_t i, const std::size_t j) override {
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

private:
    std::vector<T *> data_;
    std::vector<std::size_t> lens_;
};

// -------------------------------------------------------------------------------------------------
// Specialization for CPLXSXP, std::complex<double>

template<>
class ListOutputWrapper<CPLXSXP, std::complex<double>> : public OutputWrapper<std::complex<double>> {
public:
    ListOutputWrapper(Rcpp::List data);

    virtual std::complex<double>& operator()(const std::size_t i, const std::size_t j) override;

private:
    std::vector<std::complex<double> *> data_;
    std::vector<std::size_t> lens_;
};

// =================================================================================================

template<R_vec_t RT, typename T>
class DataFrameOutputWrapper : public OutputWrapper<T> {
public:
    DataFrameOutputWrapper(Rcpp::DataFrame data)
        : cols_(data.ncol())
        , lens_(data.ncol())
    {
        for (R_xlen_t j = 0; j < data.ncol(); j++) {
            Rcpp::Vector<RT> one_vec(data[j]);
            lens_[j] = one_vec.length();
            if (one_vec.length() > 0) {
                cols_[j] = &one_vec[0];
            }
        }
    }

    virtual T& operator()(const std::size_t i, const std::size_t j) override {
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

private:
    std::vector<T *> cols_;
    std::vector<std::size_t> lens_;
};

// -------------------------------------------------------------------------------------------------
// Specialization for CPLXSXP, std::complex<double>

template<>
class DataFrameOutputWrapper<CPLXSXP, std::complex<double>> : public OutputWrapper<std::complex<double>> {
public:
    DataFrameOutputWrapper(Rcpp::DataFrame data);

    virtual std::complex<double>& operator()(const std::size_t i, const std::size_t j) override;

private:
    std::vector<std::complex<double> *> cols_;
    std::vector<std::size_t> lens_;
};

// =================================================================================================

template<int RT, typename T>
class MatrixOutputWrapper : public OutputWrapper<T> {
public:
    MatrixOutputWrapper(Rcpp::Matrix<RT> data)
        : ncol_(data.ncol())
        , nrow_(data.nrow())
        , data_(&data[0])
    {
        // nocov start
        if (ncol_ == 0 || nrow_ == 0) {
            throw std::invalid_argument("[wiserow] output matrix is empty.");
        }
        // nocov end
    }

    virtual T& operator()(const std::size_t i, const std::size_t j) override {
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

private:
    const std::size_t ncol_;
    const std::size_t nrow_;
    T * const data_;
};

// -------------------------------------------------------------------------------------------------
// Specialization for CPLXSXP, std::complex<double>

template<>
class MatrixOutputWrapper<CPLXSXP, std::complex<double>> : public OutputWrapper<std::complex<double>> {
public:
    MatrixOutputWrapper(Rcpp::Matrix<CPLXSXP> data);

    virtual std::complex<double>& operator()(const std::size_t i, const std::size_t j) override;

private:
    const std::size_t ncol_;
    const std::size_t nrow_;
    std::complex<double> * const data_;
};

} // namespace wiserow

#endif // WISEROW_OUTPUTWRAPPER_H_
