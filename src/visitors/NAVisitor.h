#ifndef WISEROW_NAVISITOR_H_
#define WISEROW_NAVISITOR_H_

#include <complex>
#include <memory>

#include <boost/utility/string_ref.hpp>

#include "BooleanVisitorDecorator.h"

namespace wiserow {

class NAVisitor : public BooleanVisitorDecorator
{
public:
    NAVisitor();
    NAVisitor(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor, const bool negate);

    bool operator()(const int val) const override;
    bool operator()(const double val) const override;
    bool operator()(const boost::string_ref val) const override;
    bool operator()(const std::complex<double>& val) const override;
};

} // namespace wiserow

#endif // WISEROW_NAVISITOR_H_
