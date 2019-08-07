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
    return forward(val, Rcpp::IntegerVector::is_na(val));
}

bool NAVisitor::operator()(const double val) const {
    return forward(val, Rcpp::NumericVector::is_na(val));
}

bool NAVisitor::operator()(const boost::string_ref val) const {
    return forward(val, val.data() == na_string_ptr);
}

bool NAVisitor::operator()(const std::complex<double>& val) const {
    return forward(val, (*this)(val.real()) || (*this)(val.imag()));
}

// =================================================================================================
// InfiniteVisitor

InfiniteVisitor::InfiniteVisitor(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor)
    : BooleanVisitorDecorator(op, visitor)
{ }

bool InfiniteVisitor::operator()(const int val) const {
    return forward(val, false);
}

bool InfiniteVisitor::operator()(const double val) const {
    return forward(val, !na_visitor_(val) && !std::isfinite(val));
}

bool InfiniteVisitor::operator()(const boost::string_ref val) const {
    return forward(val, false);
}

bool InfiniteVisitor::operator()(const std::complex<double>& val) const {
    return forward(val, !na_visitor_(val) && (!std::isfinite(val.real()) || !std::isfinite(val.imag())));
}

} // namespace wiserow
