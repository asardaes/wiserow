#include "visitors.h"

#include <Rcpp.h>

namespace wiserow {

static const char *na_string_ptr = CHAR(Rf_asChar(NA_STRING));

bool NAVisitor::operator()(const int val) const {
    return Rcpp::IntegerVector::is_na(val);
}

bool NAVisitor::operator()(const double val) const {
    return Rcpp::NumericVector::is_na(val);
}

bool NAVisitor::operator()(const boost::string_ref val) const {
    return val.data() == na_string_ptr;
}

} // namespace wiserow
