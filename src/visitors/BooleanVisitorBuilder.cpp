#include "BooleanVisitorBuilder.h"

#include "boolean-visitors.h"

namespace wiserow {

BooleanVisitorBuilder::BooleanVisitorBuilder(const BoolOp op, const bool init)
    : op_(op)
    , visitor_(std::make_shared<InitBooleanVisitor>(init))
{ }

BooleanVisitorBuilder& BooleanVisitorBuilder::is_na(const bool negate) {
    visitor_ = std::make_shared<NAVisitor>(op_, visitor_, negate);
    return *this;
}

BooleanVisitorBuilder& BooleanVisitorBuilder::is_inf(const bool negate) {
    visitor_ = std::make_shared<InfiniteVisitor>(op_, visitor_, negate);
    return *this;
}

std::shared_ptr<BooleanVisitor> BooleanVisitorBuilder::build() {
    return visitor_;
}

} // namespace wiserow
