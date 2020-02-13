#include "boolean-visitors.h"

#include <algorithm> // find

namespace wiserow {

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
