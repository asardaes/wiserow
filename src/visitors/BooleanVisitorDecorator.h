#ifndef WISEROW_BOOLEANVISITORDECORATOR_H_
#define WISEROW_BOOLEANVISITORDECORATOR_H_

#include <complex>
#include <memory>

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

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

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

} // namespace wiserow

#endif // WISEROW_BOOLEANVISITORDECORATOR_H_
