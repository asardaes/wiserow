#include "columns.h"

#include <typeinfo>
#include <typeindex>

namespace wiserow {

// =================================================================================================

ColumnCollection ColumnCollection::coerce(OperationMetadata metadata, SEXP data) {
    if (metadata.input_class == "matrix") {
        if (metadata.input_modes[0] == typeid(int)) {
            return MatrixColumnCollection<Rcpp::IntegerMatrix>(data);
        }
        else {
            return MatrixColumnCollection<Rcpp::NumericMatrix>(data);
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
