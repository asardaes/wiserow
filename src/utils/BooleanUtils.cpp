#include "BooleanUtils.h"

#include <stdexcept>

namespace wiserow {

CompOp parse_comp_op(const std::string& comp_op) {
    if (comp_op == "==") {
        return CompOp::EQ;
    }
    else if (comp_op == "!=") {
        return CompOp::NEQ;
    }
    else if (comp_op == "<") {
        return CompOp::LT;
    }
    else if (comp_op == "<=") {
        return CompOp::LTE;
    }
    else if (comp_op == ">") {
        return CompOp::GT;
    }
    else if (comp_op == ">=") {
        return CompOp::GTE;
    }
    else {
        throw std::invalid_argument("[wiserow] invalid comparison operand."); // nocov - checked in R
    }
}

// =================================================================================================

LogicalOperator::LogicalOperator(const BoolOp bool_op)
    : bool_op_(bool_op)
{ }

bool LogicalOperator::apply(bool a, bool b) const {
    switch(bool_op_) {
    case BoolOp::AND:
        return a && b;
    case BoolOp::OR:
        return a || b;
    }

    return false; // nocov
}

// =================================================================================================

ComparisonOperator::ComparisonOperator(const CompOp comp_op)
    : comp_op_(comp_op)
{ }

bool ComparisonOperator::apply(const boost::string_ref& a, const boost::string_ref& b) const {
    return this->apply<boost::string_ref, boost::string_ref>(a, b);
}

bool ComparisonOperator::apply(const std::complex<double>& a, const std::complex<double>& b) const {
    switch(comp_op_) {
    case CompOp::EQ:
        return a == b;
    case CompOp::NEQ:
        return a != b;
    default:
        throw std::invalid_argument("[wiserow] complex numbers can only be compared for (in)equality.");
    }
}

bool ComparisonOperator::apply(const std::complex<double>& a, const double b) const {
    switch(comp_op_) {
    case CompOp::EQ:
        return a.imag() == 0 && a.real() == b;
    case CompOp::NEQ:
        return a.imag() != 0 || a.real() != b;
    default:
        throw std::invalid_argument("[wiserow] complex numbers can only be compared for (in)equality.");
    }
}

bool ComparisonOperator::apply(const double a, const std::complex<double>& b) const {
    return this->apply(b, a);
}

bool ComparisonOperator::apply(const std::complex<double>& a, const int b) const {
    return this->apply(a, static_cast<double>(b));
}

bool ComparisonOperator::apply(const int a, const std::complex<double>& b) const {
    return this->apply(b, static_cast<double>(a));
}

} // namespace wiserow
