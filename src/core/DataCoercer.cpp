#include "DataCoercer.h"

#include <typeinfo>
#include <typeindex>

namespace wiserow {

ColumnCollection DataCoercer::coerce(Metadata metadata, SEXP data) {
    if (metadata.input_class == "matrix") {
        if (metadata.input_modes[0] == typeid(int)) {
            return MatrixColumnCollection<Rcpp::IntegerMatrix>(data);
        }
        else {
            return MatrixColumnCollection<Rcpp::NumericMatrix>(data);
        }
    }
    else {
        Rcpp::stop("Unknown input class: " + metadata.input_class);
    }
}

} // namespace wiserow
