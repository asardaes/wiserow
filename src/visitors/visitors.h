#ifndef WISEROW_VISITORS_H_
#define WISEROW_VISITORS_H_

#include <complex>
#include <stdexcept> // invalid_argument

#include <boost/utility/string_ref.hpp>
#include <boost/variant/static_visitor.hpp>

namespace wiserow {

// =================================================================================================

class NAVisitor : public boost::static_visitor<bool>
{
public:
    NAVisitor() {}

    bool operator()(const int val) const;
    bool operator()(const double val) const;
    bool operator()(const boost::string_ref val) const;
    bool operator()(const std::complex<double>& val) const;
};

// =================================================================================================

template<typename T>
class NumericVisitor : public boost::static_visitor<T>
{
public:
    NumericVisitor() {}

    template<typename U>
    T operator()(const U val) const {
        return val;
    }

    T operator()(const boost::string_ref val) const {
        throw std::invalid_argument("[wiserow] this operation does not support characters.");
    }

    T operator()(const std::complex<double>& val) const {
        return std::abs(val);
    }
};

// -------------------------------------------------------------------------------------------------
// Specialization for std::complex<double>
template<>
class NumericVisitor<std::complex<double>> : public boost::static_visitor<std::complex<double>>
{
public:
    NumericVisitor() {}

    template<typename U>
    std::complex<double> operator()(const U& val) const {
        return val;
    }

    std::complex<double> operator()(const boost::string_ref val) const {
        throw std::invalid_argument("[wiserow] this operation does not support characters.");
    }
};

} // namespace wiserow

#endif // WISEROW_VISITORS_H_
