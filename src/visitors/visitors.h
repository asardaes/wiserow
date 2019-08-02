#ifndef WISEROW_VISITORS_H_
#define WISEROW_VISITORS_H_

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
};

} // namespace wiserow

#endif // WISEROW_VISITORS_H_
