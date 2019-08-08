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

BooleanVisitorDecorator::BooleanVisitorDecorator(const BoolOp op,
                                                 const std::shared_ptr<BooleanVisitor>& visitor,
                                                 const bool negate)
    : op_(op)
    , parent_(visitor)
    , negate_(negate)
{ }

bool BooleanVisitorDecorator::short_circuit(const bool val) const {
    if ((op_ == BoolOp::AND && !val) || (op_ == BoolOp::OR && val)) {
        return true;
    }
    else {
        return false;
    }
}

// if this is called, it means short_circuit was already tested
bool BooleanVisitorDecorator::forward(const bool ans) const {
    return ans ^ negate_; // XOR
}

} // namespace wiserow
