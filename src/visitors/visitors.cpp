#include "../visitors.h"

#include <cmath> // isfinite

#include <Rcpp.h>

namespace wiserow {

// Singleton, if address is same, string is NA
static const char *na_string_ptr = CHAR(Rf_asChar(NA_STRING));

// =================================================================================================
// NAVisitor

NAVisitor::NAVisitor()
    : BooleanVisitorDecorator(BoolOp::OR, std::make_shared<InitBooleanVisitor>(false))
{ }

NAVisitor::NAVisitor(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor)
    : BooleanVisitorDecorator(op, visitor)
{ }

bool NAVisitor::operator()(const int val) const {
    return forward(Rcpp::IntegerVector::is_na(val));
}

bool NAVisitor::operator()(const double val) const {
    return forward(Rcpp::NumericVector::is_na(val));
}

bool NAVisitor::operator()(const boost::string_ref val) const {
    return forward(val.data() == na_string_ptr);
}

bool NAVisitor::operator()(const std::complex<double>& val) const {
    return forward((*this)(val.real()) || (*this)(val.imag()));
}

} // namespace wiserow
