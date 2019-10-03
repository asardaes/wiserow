#include "../visitors.h"

#include <algorithm> // find
#include <cmath> // isfinite

#include <Rcpp.h>

#include "boolean-visitors.h"

namespace wiserow {

// Singleton, if address is same, string is NA
static const char *na_string_ptr = CHAR(Rf_asChar(NA_STRING));

// =================================================================================================
// NAVisitor

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

// =================================================================================================
// InfiniteVisitor

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

// =================================================================================================

InSetVisitor<std::complex<double>>::InSetVisitor(const BoolOp bool_op,
                                                 const std::shared_ptr<BooleanVisitor>& visitor,
                                                 const bool negate,
                                                 const std::complex<double> * const target_vals,
                                                 const std::size_t n_target_vals)
    : BooleanVisitorDecorator(bool_op, visitor, negate)
    , any_target_na_(false)
{
    for (std::size_t i = 0; i < n_target_vals; i++) {
        if (na_visitor_(target_vals[i])) {
            any_target_na_ = true;
        }
        else {
            target_vals_.push_back(target_vals[i]);
            target_string_vals_.insert(::wiserow::to_string(target_vals[i]));
        }
    }
}

bool InSetVisitor<std::complex<double>>::operator()(const int val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    if (na_visitor_(val)) return forward(any_target_na_);
    return forward(std::find(target_vals_.begin(), target_vals_.end(), std::complex<double>(val, 0)) != target_vals_.end());
}

bool InSetVisitor<std::complex<double>>::operator()(const double val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    if (na_visitor_(val)) return forward(any_target_na_);
    return forward(std::find(target_vals_.begin(), target_vals_.end(), std::complex<double>(val, 0)) != target_vals_.end());
}

bool InSetVisitor<std::complex<double>>::operator()(const boost::string_ref val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    if (na_visitor_(val)) return forward(any_target_na_);
    return forward(target_string_vals_.find(val.to_string()) != target_string_vals_.end());
}

bool InSetVisitor<std::complex<double>>::operator()(const std::complex<double>& val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    if (na_visitor_(val)) return forward(any_target_na_);
    return forward(std::find(target_vals_.begin(), target_vals_.end(), val) != target_vals_.end());
}

// -------------------------------------------------------------------------------------------------

InSetVisitor<std::string>::InSetVisitor(const BoolOp bool_op,
                                        const std::shared_ptr<BooleanVisitor>& visitor,
                                        const bool negate,
                                        std::unordered_set<std::string>&& target_vals,
                                        const bool include_na)
    : BooleanVisitorDecorator(bool_op, visitor, negate)
    , target_vals_(target_vals)
    , include_na_(include_na)
{ }

bool InSetVisitor<std::string>::operator()(const int val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;

    if (na_visitor_(val)) return forward(include_na_);

    std::string str_val = ::wiserow::to_string(val);
    return forward(target_vals_.find(str_val) != target_vals_.end());
}

bool InSetVisitor<std::string>::operator()(const double val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;

    if (na_visitor_(val)) return forward(include_na_);

    std::string str_val = ::wiserow::to_string(val);
    return forward(target_vals_.find(str_val) != target_vals_.end());
}

bool InSetVisitor<std::string>::operator()(const boost::string_ref val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;

    if (na_visitor_(val)) return forward(include_na_);
    return forward(target_vals_.find(val.to_string()) != target_vals_.end());
}

bool InSetVisitor<std::string>::operator()(const std::complex<double>& val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;

    if (na_visitor_(val)) return forward(include_na_);

    std::string str_val = ::wiserow::to_string(val);
    return forward(target_vals_.find(str_val) != target_vals_.end());
}

} // namespace wiserow
