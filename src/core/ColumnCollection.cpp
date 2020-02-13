#include "ColumnAbstractions.h"

#include "DataFrameColumnCollection.h"
#include "MatrixColumnCollection.h"

namespace wiserow {

ColumnCollection ColumnCollection::coerce(const OperationMetadata& metadata, SEXP data) {
    switch(metadata.input_class) {
    case RClass::MATRIX: {
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
    case RClass::DATAFRAME: {
        return DataFrameColumnCollection(data, metadata);
    }
    default: { // nocov start
        // can never happen because enums, wtf gcc?
        Rcpp::stop("[wiserow] this should never happen D=");
    } // nocov end
    }
}

// -------------------------------------------------------------------------------------------------

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

} // namespace wiserow
