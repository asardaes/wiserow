#include "BooleanVisitorDecorator.h"

namespace wiserow {

InitBooleanVisitor::InitBooleanVisitor(const bool init)
    : init_(init)
{ }

bool InitBooleanVisitor::operator()(const int val) const {
    return init_;
}

bool InitBooleanVisitor::operator()(const double val) const {
    return init_;
}

bool InitBooleanVisitor::operator()(const boost::string_ref val) const {
    return init_;
}

bool InitBooleanVisitor::operator()(const std::complex<double>& val) const {
    return init_;
}

// -------------------------------------------------------------------------------------------------

BooleanVisitorDecorator::BooleanVisitorDecorator(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor)
    : op_(op)
    , visitor_(visitor)
{ }

bool BooleanVisitorDecorator::operator()(const int val) const {
    return delegate(val);
}

bool BooleanVisitorDecorator::operator()(const double val) const {
    return delegate(val);
}

bool BooleanVisitorDecorator::operator()(const boost::string_ref val) const {
    return delegate(val);
}

bool BooleanVisitorDecorator::operator()(const std::complex<double>& val) const {
    return delegate(val);
}

} // namespace wiserow
