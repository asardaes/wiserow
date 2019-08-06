#ifndef WISEROW_BOOLEANVISITORBUILDER_H_
#define WISEROW_BOOLEANVISITORBUILDER_H_

#include "BooleanVisitorDecorator.h"
#include "NAVisitor.h"

#include "../utils.h"

namespace wiserow {

class BooleanVisitorBuilder
{
public:
    BooleanVisitorBuilder(const bool init = false);

    BooleanVisitorBuilder& is_na(const BoolOp op);

    std::shared_ptr<BooleanVisitor> build();

private:
    std::shared_ptr<BooleanVisitor> visitor_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANVISITORBUILDER_H_
