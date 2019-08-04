#ifndef WISEROW_NAVISITOR_H_
#define WISEROW_NAVISITOR_H_

#include <complex>

#include <boost/utility/string_ref.hpp>
#include <boost/variant/static_visitor.hpp>

namespace wiserow {

class NAVisitor : public boost::static_visitor<bool>
{
public:
    NAVisitor() {}

    bool operator()(const int val) const;
    bool operator()(const double val) const;
    bool operator()(const boost::string_ref val) const;
    bool operator()(const std::complex<double>& val) const;
};

} // namespace wiserow

#endif // WISEROW_NAVISITOR_H_
