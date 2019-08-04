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

// =================================================================================================

// nocov start
template<typename T>
class OutputWrapper {
public:
    virtual ~OutputWrapper() {}

    virtual T& operator[](const std::size_t i) {
        return (*this)(i, 0);
    }

    virtual T& operator()(const std::size_t i, const std::size_t j) {
        throw std::runtime_error("[wiserow] unsupported.");
    }
};
// nocov end

// =================================================================================================

template<int RT, typename T>
class VectorOutputWrapper : public OutputWrapper<T> {
public:
    VectorOutputWrapper(Rcpp::Vector<RT> data)
        : data_(&data[0])
        , len_(data.length())
    { }

    virtual T& operator[](const std::size_t i) override {
        if (i >= len_) { // nocov start
            throw std::out_of_range("[wiserow] attempted to index a vector of length " +
                                    std::to_string(len_) +
                                    " at " +
                                    std::to_string(i + 1));
        } // nocov end

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
    VectorOutputWrapper(Rcpp::ComplexVector vec)
        : data_(reinterpret_cast<std::complex<double> *>(&vec[0]))
        , len_(vec.length())
    { }

    virtual std::complex<double>& operator[](const std::size_t i) override {
        if (i >= len_) { // nocov start
            throw std::out_of_range("[wiserow] attempted to index a vector of length " +
                                    std::to_string(len_) +
                                    " at " +
                                    std::to_string(i + 1));
        } // nocov end

        return data_[i];
    }

private:
    std::complex<double> * const data_;
    const std::size_t len_;
};

// =================================================================================================

template<int RT, typename T>
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
    ListOutputWrapper(Rcpp::List data)
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

    virtual std::complex<double>& operator()(const std::size_t i, const std::size_t j) override {
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
    std::vector<std::complex<double> *> data_;
    std::vector<std::size_t> lens_;
};

} // namespace wiserow

#endif // WISEROW_OUTPUTWRAPPER_H_
