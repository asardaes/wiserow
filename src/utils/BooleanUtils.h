#ifndef WISEROW_BOOLEANUTILS_H_
#define WISEROW_BOOLEANUTILS_H_

#include <complex>
#include <stdexcept> // invalid_argument

#include <boost/utility/string_ref.hpp>

#include "StringUtils.h"

namespace wiserow {

enum class BoolOp {
    AND,
    OR
};

enum class BulkBoolOp {
    ALL,
    ANY,
    NONE
};

enum class CompOp {
    EQ,     // ==
    NEQ,    // !=
    LT,     // <
    LTE,    // <=
    GT,     // >
    GTE     // >=
};

CompOp parse_comp_op(const std::string& comp_op);

// =================================================================================================

class LogicalOperator {
public:
    LogicalOperator(const BoolOp bool_op);

    bool apply(bool a, bool b) const;

private:
    const BoolOp bool_op_;
};

// =================================================================================================

// will NOT deal with NA
class ComparisonOperator {
public:
    ComparisonOperator(const CompOp comp_op)
        : comp_op_(comp_op)
    { }

    template<typename T, typename U>
    bool apply(const T& a, const U& b) const {
        switch(comp_op_) {
        case CompOp::EQ:
            return a == b;
        case CompOp::NEQ:
            return a != b;
        case CompOp::LT:
            return a < b;
        case CompOp::LTE:
            return a <= b;
        case CompOp::GT:
            return a > b;
        case CompOp::GTE:
            return a >= b;
        }

        return false; // nocov
    }

    bool apply(const boost::string_ref& a, const boost::string_ref& b) const {
        return this->apply<boost::string_ref, boost::string_ref>(a, b);
    }

    template<typename T>
    bool apply(const boost::string_ref& a, const T& b) const {
        switch(comp_op_) {
        case CompOp::EQ:
            return a == ::wiserow::to_string(b);
        case CompOp::NEQ:
            return a != ::wiserow::to_string(b);
        case CompOp::LT:
            return a < ::wiserow::to_string(b);
        case CompOp::LTE:
            return a <= ::wiserow::to_string(b);
        case CompOp::GT:
            return a > ::wiserow::to_string(b);
        case CompOp::GTE:
            return a >= ::wiserow::to_string(b);
        }

        return false; // nocov
    }

    template<typename T>
    bool apply(const T& a, const boost::string_ref& b) const {
        switch(comp_op_) {
        case CompOp::EQ:
            return ::wiserow::to_string(a) == b;
        case CompOp::NEQ:
            return ::wiserow::to_string(a) != b;
        case CompOp::LT:
            return ::wiserow::to_string(a) < b;
        case CompOp::LTE:
            return ::wiserow::to_string(a) <= b;
        case CompOp::GT:
            return ::wiserow::to_string(a) > b;
        case CompOp::GTE:
            return ::wiserow::to_string(a) >= b;
        }

        return false; // nocov
    }

    bool apply(const std::complex<double>& a, const std::complex<double>& b) const {
        switch(comp_op_) {
        case CompOp::EQ:
            return a == b;
        case CompOp::NEQ:
            return a != b;
        default:
            throw std::invalid_argument("[wiserow] complex numbers can only be compared for (in)equality.");
        }
    }

    bool apply(const std::complex<double>& a, const double b) const {
        switch(comp_op_) {
        case CompOp::EQ:
            return a.imag() == 0 && a.real() == b;
        case CompOp::NEQ:
            return a.imag() != 0 || a.real() != b;
        default:
            throw std::invalid_argument("[wiserow] complex numbers can only be compared for (in)equality.");
        }
    }

    bool apply(const double a, const std::complex<double>& b) const {
        return this->apply(b, a);
    }

    bool apply(const std::complex<double>& a, const int b) const {
        return this->apply(a, static_cast<double>(b));
    }

    bool apply(const int a, const std::complex<double>& b) const {
        return this->apply(b, static_cast<double>(a));
    }

private:
    const CompOp comp_op_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANUTILS_H_
