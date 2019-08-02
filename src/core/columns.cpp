#include "columns.h"

#include <stdexcept> // out_of_range
#include <string>
#include <vector>

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

// -------------------------------------------------------------------------------------------------
// Specialization for Rcpp::StringMatrix
template<>
class SurrogateColumn<Rcpp::StringMatrix> : public VariantColumn
{
public:
    SurrogateColumn(const Rcpp::StringMatrix& mat, const int j, const std::size_t size)
        : data_ptr_(size)
        , size_(size)
    {
        for (std::size_t i = 0; i < size; i++) {
            // https://stackoverflow.com/a/7875438/5793905
            Rcpp::CharacterVector one_string = Rcpp::as<Rcpp::CharacterVector>(mat[i + j * size]);
            data_ptr_[i] = (char *)(one_string[0]);
        }
    }

    const supported_col_t operator[](const std::size_t id) const override {
        if (id >= size_) { // nocov start
            throw std::out_of_range("Column of size " +
                                    std::to_string(size_) +
                                    " cannot be indexed at " +
                                    std::to_string(id));
        } // nocov end

        return supported_col_t(std::string(data_ptr_[id]));
    }

private:
    std::vector<char *> data_ptr_;
    const std::size_t size_;
};

// =================================================================================================

template<int RT, typename T>
class MatrixColumnCollection : public ColumnCollection
{
public:
    MatrixColumnCollection(const Rcpp::Matrix<RT>& mat)
        : ColumnCollection(mat.nrow())
    {
        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(std::make_shared<SurrogateColumn<T>>(&mat[j * nrow_], nrow_));
        }
    }
};

// -------------------------------------------------------------------------------------------------
// Specialization for STRSXP
template<typename T>
class MatrixColumnCollection<STRSXP, T> : public ColumnCollection
{
public:
    MatrixColumnCollection(const Rcpp::Matrix<STRSXP>& mat)
        : ColumnCollection(mat.nrow())
    {
        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringMatrix>>(mat, j, nrow_));
        }
    }
};

// =================================================================================================

ColumnCollection ColumnCollection::coerce(const OperationMetadata& metadata, SEXP data) {
    if (metadata.input_class == "matrix") {
        switch(metadata.input_modes[0]) {
        case INTSXP: {
            return MatrixColumnCollection<INTSXP, int>(data);
        }
        case REALSXP: {
            return MatrixColumnCollection<REALSXP, double>(data);
        }
        case LGLSXP: {
            return MatrixColumnCollection<LGLSXP, int>(data);
        }
        case STRSXP: {
            return MatrixColumnCollection<STRSXP, const char *>(data);
        }
        default: {
            Rcpp::stop("[wiserow] matrices can only contain integers, doubles, logicals, or characters.");
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
