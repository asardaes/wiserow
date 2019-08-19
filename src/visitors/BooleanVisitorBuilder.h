#ifndef WISEROW_BOOLEANVISITORBUILDER_H_
#define WISEROW_BOOLEANVISITORBUILDER_H_

#define R_NO_REMAP
#include <Rinternals.h> // SEXP

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
    BooleanVisitorBuilder& compare(const CompOp comp_op, const SEXP& target_val);

    std::shared_ptr<BooleanVisitor> build();

private:
    const BoolOp op_;
    std::shared_ptr<BooleanVisitor> visitor_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANVISITORBUILDER_H_
