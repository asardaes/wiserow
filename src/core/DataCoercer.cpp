#include "DataCoercer.h"

#include <typeinfo>
#include <typeindex>

namespace wiserow {

ColumnCollection DataCoercer::coerce(Metadata metadata, SEXP data) {
    if (metadata.input_class == "matrix") {
        if (metadata.input_modes[0] == typeid(int)) {
            Rcpp::IntegerMatrix mat(data);
            return MatrixColumnCollection<Rcpp::IntegerMatrix, int>(mat);
        }
        else {
            Rcpp::NumericMatrix mat(data);
            return MatrixColumnCollection<Rcpp::NumericMatrix, double>(mat);
        }
    }
    else {
        Rcpp::stop("Unknown input class: " + metadata.input_class);
    }
}

} // namespace wiserow
