#ifndef WISEROW_SURROGATECOLUMN_H_
#define WISEROW_SURROGATECOLUMN_H_

#include <complex>
#include <cstddef> // size_t
#include <stdexcept> // out_of_range
#include <string> // to_string
#include <vector>

#include <boost/utility/string_ref.hpp>
#include <Rcpp.h>

#include "columns.h"

namespace wiserow {

template<typename T>
class SurrogateColumn : public VariantColumn
{
public:
    SurrogateColumn(T const * const data_ptr, const std::size_t size)
        : data_ptr_(data_ptr)
        , size_(size)
    { }

    const supported_col_t operator[](const std::size_t id) const override {
        if (id >= size_) { // nocov start
            throw std::out_of_range("[wiserow] column of size " +
                                    std::to_string(size_) +
                                    " cannot be indexed at " +
                                    std::to_string(id));
        } // nocov end

        return supported_col_t(data_ptr_[id]);
    }

private:
    T const * const data_ptr_;
    const std::size_t size_;
};

// -------------------------------------------------------------------------------------------------
// Specialization for Rcpp::StringMatrix
template<>
class SurrogateColumn<Rcpp::StringMatrix> : public VariantColumn
{
public:
    SurrogateColumn(const Rcpp::StringMatrix& mat, const int j, const std::size_t size);

    const supported_col_t operator[](const std::size_t id) const override;

private:
    std::vector<char *> data_ptr_;
    const std::size_t size_;
};

// -------------------------------------------------------------------------------------------------
// Specialization for Rcpp::ComplexMatrix
// see http://rcpp-devel.r-forge.r-project.narkive.com/o5ubHVos/multiplication-of-complexvector
template<>
class SurrogateColumn<Rcpp::ComplexMatrix> : public VariantColumn
{
public:
    SurrogateColumn(const Rcpp::ComplexMatrix& mat, const int j, const std::size_t size);

    const supported_col_t operator[](const std::size_t id) const override;

private:
    const std::complex<double> *data_ptr_;
    const std::size_t size_;
};

} // namespace wiserow

#endif // WISEROW_SURROGATECOLUMN_H_
