#ifndef WISEROW_BOOLEANUTILS_H_
#define WISEROW_BOOLEANUTILS_H_

#include <complex>
#include <stdexcept> // invalid_argument
#include <type_traits> // is_same

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

class LogicalOperator
{
public:
    LogicalOperator(const BoolOp bool_op);

    bool apply(bool a, bool b) const;

private:
    const BoolOp bool_op_;
};

// =================================================================================================

// will NOT deal with NA
class ComparisonOperator
{
public:
    ComparisonOperator(const CompOp comp_op);

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

    bool apply(const boost::string_ref& a, const boost::string_ref& b) const;

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

    bool apply(const std::complex<double>& a, const std::complex<double>& b) const;

    bool apply(const std::complex<double>& a, const double b) const;

    bool apply(const double a, const std::complex<double>& b) const;

    bool apply(const std::complex<double>& a, const int b) const;

    bool apply(const int a, const std::complex<double>& b) const;

    template<typename T>
    typename std::enable_if<!std::is_same<T, boost::string_ref>::value, bool>::type
    apply(const T a, const bool b) const {
        return this->apply(a, static_cast<int>(b));
    }

private:
    const CompOp comp_op_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANUTILS_H_
