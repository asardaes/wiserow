#include "BooleanVisitorBuilder.h"

namespace wiserow {

BooleanVisitorBuilder::BooleanVisitorBuilder(const bool init)
    : visitor_(std::make_shared<InitBooleanVisitor>(init))
{ }

BooleanVisitorBuilder& BooleanVisitorBuilder::is_na(const BoolOp op) {
    visitor_ = std::make_shared<NAVisitor>(op, visitor_);
    return *this;
}

std::shared_ptr<BooleanVisitor> BooleanVisitorBuilder::build() {
    return visitor_;
}

} // namespace wiserow
