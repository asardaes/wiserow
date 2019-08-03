#include "columns.h"

#include <stdexcept> // out_of_range
#include <string> // to_string

#include <Rcpp.h>

#include "MatrixColumnCollection.h"

namespace wiserow {

ColumnCollection ColumnCollection::coerce(const OperationMetadata& metadata, SEXP data) {
    if (metadata.input_class == "matrix") {
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

const supported_col_t ColumnCollection::operator()(const std::size_t i, const std::size_t j) const {
    return (*(columns_[j]))[i];
}

} // namespace wiserow
