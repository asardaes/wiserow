#ifndef WISEROW_BOOLEANVISITORBUILDER_H_
#define WISEROW_BOOLEANVISITORBUILDER_H_

#include "BooleanVisitorDecorator.h"
#include "NAVisitor.h"

#include "../utils.h"

namespace wiserow {

class BooleanVisitorBuilder
{
public:
    BooleanVisitorBuilder(const bool init = false)
        : visitor_(std::make_shared<InitBooleanVisitor>(init))
    { }

    BooleanVisitorBuilder& is_na(const BoolOp op) {
        visitor_ = std::make_shared<NAVisitor>(op, visitor_);
        return *this;
    }

    std::shared_ptr<BooleanVisitor> build() {
        return visitor_;
    }

private:
    std::shared_ptr<BooleanVisitor> visitor_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANVISITORBUILDER_H_
