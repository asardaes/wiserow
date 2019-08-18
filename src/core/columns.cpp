#include "ColumnAbstractions.h"
#include "DataFrameColumnCollection.h"
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
            Rcpp::stop("[wiserow] matrices can only contain integers, doubles, logicals, characters, or complex.");
        } // nocov end
        }
    }
    case InputClass::DATAFRAME: {
        return DataFrameColumnCollection(data, metadata);
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

std::shared_ptr<const VariantColumn> ColumnCollection::operator[](const std::size_t j) const {
    return columns_[j];
}

const supported_col_t ColumnCollection::operator()(const std::size_t i, const std::size_t j) const {
    return (*(columns_[j]))[i];
}

// =================================================================================================

DataFrameColumnCollection::DataFrameColumnCollection(const Rcpp::DataFrame& df, const OperationMetadata& metadata)
    : ColumnCollection(df.nrow())
{
    int upper_j = 0,
        current_j = 0;

    if (metadata.cols.ptr) {
        upper_j = static_cast<int>(metadata.cols.len);
    }
    else if (metadata.cols.is_null) {
        upper_j = df.ncol();
    }

    for (int j = 0; j < upper_j; j++) {
        current_j = metadata.cols.ptr ? metadata.cols.ptr[j] - 1 : j;

        switch(metadata.input_modes[current_j]) {
        case INTSXP: {
            Rcpp::IntegerVector vec(df[current_j]);

            if (!Rf_isFactor(df[current_j]) || metadata.factor_mode == INTSXP) {
                columns_.push_back(std::make_shared<SurrogateColumn<int>>(&vec[0], vec.length()));
            }
            else {
                Rcpp::StringVector levels = vec.attr("levels");
                Rcpp::StringVector factors(vec.length());
                for (R_xlen_t i = 0; i < vec.length(); i++) {
                    if (Rcpp::IntegerVector::is_na(vec[i])) {
                        factors[i] = NA_STRING;
                    }
                    else {
                        factors[i] = levels[vec[i] - 1];
                    }
                }

                columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringVector>>(factors));
            }

            break;
        }
        case REALSXP: {
            Rcpp::NumericVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<double>>(&vec[0], vec.length()));
            break;
        }
        case LGLSXP: {
            Rcpp::LogicalVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<int>>(&vec[0], vec.length(), true));
            break;
        }
        case STRSXP: {
            Rcpp::StringVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringVector>>(vec));
            break;
        }
        case CPLXSXP: {
            Rcpp::ComplexVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexVector>>(vec));
            break;
        }
        default: { // nocov start
            // can never happen because OperationMetadata's constructor checks this too
            Rcpp::stop("[wiserow] data frames can only contain integers, doubles, logicals, characters, or complex.");
        } // nocov end
        }
    }
}


// =================================================================================================

MatrixColumnCollection<STRSXP, const char *>::MatrixColumnCollection(const Rcpp::Matrix<STRSXP>& mat,
                                                                     const surrogate_vector& cols)
    : ColumnCollection(mat.nrow())
{
    if (cols.ptr) {
        for (std::size_t i = 0; i < cols.len; i++) {
            int j = cols.ptr[i] - 1;
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringMatrix>>(mat, j));
        }
    }
    else if (cols.is_null) {
        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringMatrix>>(mat, j));
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
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexMatrix>>(mat, j));
        }
    }
    else if (cols.is_null) {
        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexMatrix>>(mat, j));
        }
    }
}

// =================================================================================================

SurrogateColumn<Rcpp::StringMatrix>::SurrogateColumn(const Rcpp::StringMatrix& mat, const int j)
    : data_ptr_(mat.nrow())
    , size_(mat.nrow())
{
    for (std::size_t i = 0; i < size_; i++) {
        // https://stackoverflow.com/a/7875438/5793905
        Rcpp::CharacterVector one_string = Rcpp::as<Rcpp::CharacterVector>(mat[i + j * size_]);
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

SurrogateColumn<Rcpp::StringVector>::SurrogateColumn(const Rcpp::StringVector& vec)
    : data_ptr_(vec.length())
    , size_(vec.length())
{
    for (std::size_t i = 0; i < size_; i++) {
        // https://stackoverflow.com/a/7875438/5793905
        Rcpp::CharacterVector one_string = Rcpp::as<Rcpp::CharacterVector>(vec[i]);
        data_ptr_[i] = (char *)(one_string[0]);
    }
}

const supported_col_t SurrogateColumn<Rcpp::StringVector>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(boost::string_ref(data_ptr_[id]));
}

// -------------------------------------------------------------------------------------------------

SurrogateColumn<Rcpp::ComplexMatrix>::SurrogateColumn(const Rcpp::ComplexMatrix& mat, const int j)
    : data_ptr_(reinterpret_cast<const std::complex<double> *>(&mat[j * mat.nrow()]))
    , size_(mat.nrow())
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

// -------------------------------------------------------------------------------------------------

SurrogateColumn<Rcpp::ComplexVector>::SurrogateColumn(const Rcpp::ComplexVector& vec)
    : data_ptr_(reinterpret_cast<const std::complex<double> *>(&vec[0]))
    , size_(vec.length())
{ }

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
