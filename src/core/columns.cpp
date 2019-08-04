#include "ColumnAbstractions.h"
#include "MatrixColumnCollection.h"
#include "SurrogateColumn.h"

#include <stdexcept> // out_of_range
#include <string> // to_string

#include <Rcpp.h>

namespace wiserow {

// =================================================================================================

ColumnCollection ColumnCollection::coerce(const OperationMetadata& metadata, SEXP data) {
    switch(metadata.input_class) {
    case InputClass::MATRIX: {
        switch(metadata.input_modes[0]) {
        case INTSXP: {
            return MatrixColumnCollection<INTSXP, int>(data, metadata.cols);
        }
        case REALSXP: {
            return MatrixColumnCollection<REALSXP, double>(data, metadata.cols);
        }
        case LGLSXP: {
            return MatrixColumnCollection<LGLSXP, int>(data, metadata.cols);
        }
        case STRSXP: {
            return MatrixColumnCollection<STRSXP, const char *>(data, metadata.cols);
        }
        case CPLXSXP: {
            return MatrixColumnCollection<CPLXSXP, std::complex<double>>(data, metadata.cols);
        }
        default: { // nocov start
            // can never happen because OperationMetadata's constructor checks this too
            Rcpp::stop("[wiserow] matrices can only contain integers, doubles, logicals, or characters.");
        } // nocov end
        }
    }
    default: { // nocov start
        // can never happen because enums, wtf gcc?
        Rcpp::stop("[wiserow] this should never happen D=");
    } // nocov end
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

const supported_col_t ColumnCollection::operator()(const std::size_t i, const std::size_t j) const {
    return (*(columns_[j]))[i];
}

// =================================================================================================

MatrixColumnCollection<STRSXP, const char *>::MatrixColumnCollection(const Rcpp::Matrix<STRSXP>& mat,
                                                                     const surrogate_vector& cols)
    : ColumnCollection(mat.nrow())
{
    if (cols.ptr) {
        for (std::size_t i = 0; i < cols.len; i++) {
            int j = cols.ptr[i] - 1;
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringMatrix>>(mat, j, nrow_));
        }
    }
    else if (cols.is_null) {
        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringMatrix>>(mat, j, nrow_));
        }
    }
}

// -------------------------------------------------------------------------------------------------

MatrixColumnCollection<CPLXSXP, std::complex<double>>::MatrixColumnCollection(const Rcpp::Matrix<CPLXSXP>& mat,
                                                                              const surrogate_vector& cols)
    : ColumnCollection(mat.nrow())
{
    if (cols.ptr) {
        for (std::size_t i = 0; i < cols.len; i++) {
            int j = cols.ptr[i] - 1;
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexMatrix>>(mat, j, nrow_));
        }
    }
    else if (cols.is_null) {
        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexMatrix>>(mat, j, nrow_));
        }
    }
}

// =================================================================================================

SurrogateColumn<Rcpp::StringMatrix>::SurrogateColumn(const Rcpp::StringMatrix& mat, const int j, const std::size_t size)
    : data_ptr_(size)
, size_(size)
{
    for (std::size_t i = 0; i < size; i++) {
        // https://stackoverflow.com/a/7875438/5793905
        Rcpp::CharacterVector one_string = Rcpp::as<Rcpp::CharacterVector>(mat[i + j * size]);
        data_ptr_[i] = (char *)(one_string[0]);
    }
}

const supported_col_t SurrogateColumn<Rcpp::StringMatrix>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(boost::string_ref(data_ptr_[id]));
}

// -------------------------------------------------------------------------------------------------

SurrogateColumn<Rcpp::ComplexMatrix>::SurrogateColumn(const Rcpp::ComplexMatrix& mat, const int j, const std::size_t size)
    : data_ptr_(reinterpret_cast<const std::complex<double> *>(&mat[j * size]))
, size_(size)
{ }

const supported_col_t SurrogateColumn<Rcpp::ComplexMatrix>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(data_ptr_[id]);
}

} // namespace wiserow
