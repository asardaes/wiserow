#include "../utils.h"

#include <stdexcept>
#include <string>

#include "../visitors.h"

namespace wiserow {

static std::shared_ptr<BooleanVisitor> inf_visitor = BooleanVisitorBuilder().is_inf().build();

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

// dummy for some templates, shouldn't be used after optimizations
std::string to_string(const boost::string_ref val) { // nocov start
    return std::string(val.data());
} // nocov end

std::string to_string(const bool val) {
    return val ? "TRUE" : "FALSE";
}

std::string to_string(const int val) {
    return std::to_string(val);
}

std::string to_string(const double val) {
    if ((*inf_visitor)(val)) {
        return val > 0 ? "Inf" : "-Inf";
    }
    else {
        // https://stackoverflow.com/a/13709929/5793905
        std::string str = std::to_string(val);
        str.erase(str.find_last_not_of("0") + 1, std::string::npos);
        str.erase(str.find_last_not_of(".") + 1, std::string::npos);
        return str;
    }
}

std::string to_string(const std::complex<double>& val) {
    double imag = val.imag();

    if ((*inf_visitor)(val)) {
        std::string real = val.real() > 0 ? "Inf" : "-Inf";

        if (imag >= 0) {
            return real + "+" + ::wiserow::to_string(imag) + "i";
        }
        else {
            return real + ::wiserow::to_string(imag) + "i";
        }
    }
    else {
        if (imag >= 0) {
            return ::wiserow::to_string(val.real()) + "+" + ::wiserow::to_string(imag) + "i";
        }
        else {
            return ::wiserow::to_string(val.real()) + ::wiserow::to_string(imag) + "i";
        }
    }
}

} // namespace wiserow
