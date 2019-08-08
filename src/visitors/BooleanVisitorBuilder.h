#ifndef WISEROW_BOOLEANVISITORBUILDER_H_
#define WISEROW_BOOLEANVISITORBUILDER_H_

#include "BooleanVisitorDecorator.h"
#include "NAVisitor.h"
#include "InfiniteVisitor.h"

#include "../utils.h"

namespace wiserow {

class BooleanVisitorBuilder
{
public:
    BooleanVisitorBuilder(const BoolOp op = BoolOp::OR, const bool init = false);

    BooleanVisitorBuilder& is_na(const bool negate = false);
    BooleanVisitorBuilder& is_inf(const bool negate = false);

    std::shared_ptr<BooleanVisitor> build();

private:
    const BoolOp op_;
    std::shared_ptr<BooleanVisitor> visitor_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANVISITORBUILDER_H_
