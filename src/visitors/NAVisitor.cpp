#include "boolean-visitors.h"

#include <Rcpp.h>

namespace wiserow {

// Singleton, if address is same, string is NA
static const char *na_string_ptr = CHAR(Rf_asChar(NA_STRING));

// -------------------------------------------------------------------------------------------------

NAVisitor::NAVisitor()
    : BooleanVisitorDecorator(BoolOp::OR, std::make_shared<InitBooleanVisitor>(false), false)
{ }

NAVisitor::NAVisitor(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor, const bool negate)
    : BooleanVisitorDecorator(op, visitor, negate)
{ }

bool NAVisitor::operator()(const int val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(Rcpp::IntegerVector::is_na(val));
}

bool NAVisitor::operator()(const double val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(Rcpp::NumericVector::is_na(val));
}

bool NAVisitor::operator()(const boost::string_ref val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(val.data() == na_string_ptr);
}

bool NAVisitor::operator()(const std::complex<double>& val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(Rcpp::NumericVector::is_na(val.real()) || Rcpp::NumericVector::is_na(val.imag()));
}

} // namespace wiserow
