#ifndef WISEROW_BOOLEANVISITOR_H_
#define WISEROW_BOOLEANVISITOR_H_

#include <complex>
#include <memory>

#define R_NO_REMAP
#include <Rinternals.h> // SEXP

#include <boost/utility/string_ref.hpp>
#include <boost/variant.hpp>

#include "../utils.h"

namespace wiserow {

class BooleanVisitor : public boost::static_visitor<bool>
{
public:
    virtual ~BooleanVisitor() {}

    virtual bool operator()(const int val) const = 0;
    virtual bool operator()(const double val) const = 0;
    virtual bool operator()(const boost::string_ref val) const = 0;
    virtual bool operator()(const std::complex<double>& val) const = 0;
};

// =================================================================================================

class InitBooleanVisitor : public BooleanVisitor
{
public:
    InitBooleanVisitor(const bool init);

    bool operator()(const int val) const override;
    bool operator()(const double val) const override;
    bool operator()(const boost::string_ref val) const override;
    bool operator()(const std::complex<double>& val) const override;

private:
    const bool init_;
};

// =================================================================================================

class BooleanVisitorDecorator : public BooleanVisitor
{
public:
    BooleanVisitorDecorator(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor, const bool negate);

protected:
    template<typename T>
    bool super(const T& val) const {
        return (*parent_)(val);
    }

    bool short_circuit(const bool val) const;
    bool forward(const bool ans) const;

private:
    const BoolOp op_;
    const std::shared_ptr<BooleanVisitor> parent_;
    const bool negate_;
};

// =================================================================================================

class BooleanVisitorBuilder
{
public:
    BooleanVisitorBuilder(const BoolOp op = BoolOp::OR, const bool init = false);

    BooleanVisitorBuilder& is_na(const bool negate = false);
    BooleanVisitorBuilder& is_inf(const bool negate = false);
    BooleanVisitorBuilder& compare(const CompOp comp_op, const SEXP& target_val);
    BooleanVisitorBuilder& in_set(const SEXP& target_vals, const bool negate = false);

    std::shared_ptr<BooleanVisitor> build();

private:
    const BoolOp op_;
    std::shared_ptr<BooleanVisitor> visitor_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANVISITOR_H_
