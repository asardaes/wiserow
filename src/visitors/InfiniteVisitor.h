#ifndef WISEROW_INFINITEVISITOR_H_
#define WISEROW_INFINITEVISITOR_H_

#include <complex>
#include <memory>

#include <boost/utility/string_ref.hpp>

#include "BooleanVisitorDecorator.h"
#include "NAVisitor.h"

namespace wiserow {

class InfiniteVisitor : public BooleanVisitorDecorator
{
public:
    InfiniteVisitor(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor, const bool negate);

    bool operator()(const int val) const override;
    bool operator()(const double val) const override;
    bool operator()(const boost::string_ref val) const override;
    bool operator()(const std::complex<double>& val) const override;

private:
    const NAVisitor na_visitor_;
};

} // namespace wiserow

#endif // WISEROW_INFINITEVISITOR_H_
