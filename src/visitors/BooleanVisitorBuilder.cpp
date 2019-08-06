#include "BooleanVisitorBuilder.h"

namespace wiserow {

BooleanVisitorBuilder::BooleanVisitorBuilder(const BoolOp op, const bool init)
    : op_(op)
    , visitor_(std::make_shared<InitBooleanVisitor>(init))
{ }

BooleanVisitorBuilder& BooleanVisitorBuilder::is_na() {
    visitor_ = std::make_shared<NAVisitor>(op_, visitor_);
    return *this;
}

std::shared_ptr<BooleanVisitor> BooleanVisitorBuilder::build() {
    return visitor_;
}

} // namespace wiserow
