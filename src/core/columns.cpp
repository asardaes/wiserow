#include "columns.h"

#include <stdexcept> // out_of_range
#include <type_traits> // conditional, is_same

namespace wiserow {

// =================================================================================================

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
            throw std::out_of_range("Column of size " +
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

// =================================================================================================

template<typename T>
class MatrixColumnCollection : public ColumnCollection
{
public:
    MatrixColumnCollection(const T& mat)
        : ColumnCollection(mat.nrow())
    {
        typedef typename std::conditional<std::is_same<T, Rcpp::IntegerMatrix>::value, int, double>::type U;

        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(
                std::make_shared<SurrogateColumn<U>>(&mat[static_cast<std::size_t>(j) * nrow_], nrow_)
            );
        }
    }
};

// =================================================================================================

ColumnCollection ColumnCollection::coerce(const OperationMetadata& metadata, SEXP data) {
    if (metadata.input_class == "matrix") {
        switch(metadata.input_modes[0]) {
        case INTSXP: {
            return MatrixColumnCollection<Rcpp::IntegerMatrix>(data);
        }
        case REALSXP: {
            return MatrixColumnCollection<Rcpp::NumericMatrix>(data);
        }
        default: {
            Rcpp::stop("[wiserow] matrices can only contain integers or doubles.");
        }
        }
    }
    else {
        Rcpp::stop("[wiserow] unsupported input class: " + metadata.input_class);
    }
}

ColumnCollection::ColumnCollection(const std::size_t nrow)
    : nrow_(nrow)
{ }

std::size_t ColumnCollection::ncol() const {
    return columns_.size();
}

std::size_t ColumnCollection::nrow() const {
    return nrow_;
}

const VariantColumn& ColumnCollection::operator[](const std::size_t j) const {
    return *(columns_[j]);
}

const supported_col_t ColumnCollection::operator()(const std::size_t i, const std::size_t j) const {
    return (*(columns_[j]))[i];
}

} // namespace wiserow
