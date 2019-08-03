#ifndef WISEROW_OUTPUTWRAPPER_H_
#define WISEROW_OUTPUTWRAPPER_H_

#include <complex>
#include <cstddef> // size_t
#include <stdexcept> // out_of_range
#include <string>

#include <Rcpp.h>

#include "OperationMetadata.h"

namespace wiserow {

// =================================================================================================

template<typename T>
class OutputWrapper {
public:
    virtual ~OutputWrapper() {}

    virtual T& operator[](const std::size_t i) = 0;
};

// =================================================================================================

template<int RT, typename T>
class VectorOutputWrapper : public OutputWrapper<T> {
public:
    VectorOutputWrapper(Rcpp::Vector<RT>& data)
        : data_(&data[0])
        , len_(data.length())
    { }

    T& operator[](const std::size_t i) override {
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

    std::complex<double>& operator[](const std::size_t i) override {
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

} // namespace wiserow

#endif // WISEROW_OUTPUTWRAPPER_H_
