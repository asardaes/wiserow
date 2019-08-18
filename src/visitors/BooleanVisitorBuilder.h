#ifndef WISEROW_BOOLEANVISITORBUILDER_H_
#define WISEROW_BOOLEANVISITORBUILDER_H_

#include "BooleanVisitorDecorator.h" // BooleanVisitor

#include "../utils.h"
#include "boolean-visitors.h"

namespace wiserow {

class BooleanVisitorBuilder
{
public:
    BooleanVisitorBuilder(const BoolOp op = BoolOp::OR, const bool init = false);

    BooleanVisitorBuilder& is_na(const bool negate = false);
    BooleanVisitorBuilder& is_inf(const bool negate = false);

    template<typename T>
    BooleanVisitorBuilder& compare(const CompOp comp_op, const T& target_val) {
        visitor_ = std::make_shared<ComparisonVisitor<T>>(op_, comp_op, target_val, visitor_);
        return *this;
    }

    std::shared_ptr<BooleanVisitor> build();

private:
    const BoolOp op_;
    std::shared_ptr<BooleanVisitor> visitor_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANVISITORBUILDER_H_
