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
    BooleanVisitorDecorator(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor);

    bool operator()(const int val) const override;
    bool operator()(const double val) const override;
    bool operator()(const boost::string_ref val) const override;
    bool operator()(const std::complex<double>& val) const override;

protected:
    template<typename T>
    bool forward(const T val, const bool ans) const {
        switch(op_) {
        case BoolOp::AND: {
            return ans && BooleanVisitorDecorator::operator()(val);
        }
        case BoolOp::OR: {
            return ans || BooleanVisitorDecorator::operator()(val);
        }
        }

        return false; // nocov
    }

private:
    const BoolOp op_;
    const std::shared_ptr<BooleanVisitor> visitor_; // think of it as parent in decorator chain
};

} // namespace wiserow

#endif // WISEROW_BOOLEANVISITORDECORATOR_H_
