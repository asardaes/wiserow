#include "boolean-visitors.h"

#include <cmath> // isfinite

#include <Rcpp.h>

namespace wiserow {

InfiniteVisitor::InfiniteVisitor(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor, const bool negate)
    : BooleanVisitorDecorator(op, visitor, negate)
{ }

bool InfiniteVisitor::operator()(const int val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(false);
}

bool InfiniteVisitor::operator()(const double val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(!na_visitor_(val) && !std::isfinite(val));
}

bool InfiniteVisitor::operator()(const boost::string_ref val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return false; // corner case, R says strings are neither infinite nor finite...
}

bool InfiniteVisitor::operator()(const std::complex<double>& val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(!na_visitor_(val) && (!std::isfinite(val.real()) || !std::isfinite(val.imag())));
}

} // namespace wiserow
