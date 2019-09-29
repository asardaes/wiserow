#ifndef WISEROW_BOOLEANVISITORS_H_
#define WISEROW_BOOLEANVISITORS_H_

#include <complex>
#include <memory> // shared_ptr
#include <string>
#include <type_traits> // is_same

#include <boost/utility/string_ref.hpp>

#include "../utils.h"
#include "BooleanVisitorDecorator.h"

namespace wiserow {

// =================================================================================================

class NAVisitor : public BooleanVisitorDecorator
{
public:
    NAVisitor();
    NAVisitor(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor, const bool negate);

    bool operator()(const int val) const override;
    bool operator()(const double val) const override;
    bool operator()(const boost::string_ref val) const override;
    bool operator()(const std::complex<double>& val) const override;
};

// =================================================================================================

class InfiniteVisitor : public BooleanVisitorDecorator
{
public:
    InfiniteVisitor(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor, const bool negate);

    bool operator()(const int val) const override;
    bool operator()(const double val) const override;
    bool operator()(const boost::string_ref val) const override;
    bool operator()(const std::complex<double>& val) const override;

private:
    const NAVisitor na_visitor_;
};

// =================================================================================================

template<typename T>
class ComparisonVisitor : public BooleanVisitorDecorator
{
public:
    ComparisonVisitor(const BoolOp bool_op,
                      const CompOp comp_op,
                      const T& target_val,
                      const std::shared_ptr<BooleanVisitor>& visitor)
        : BooleanVisitorDecorator(bool_op, visitor, false)
        , comp_op_(comp_op)
        , target_val_(target_val)
    { }

    bool operator()(const int val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;
        return comp_op_.apply(val, target_val_);
    }

    bool operator()(const double val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;
        return comp_op_.apply(val, target_val_);
    }

    bool operator()(const boost::string_ref val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;

        if (std::is_same<T, boost::string_ref>::value) {
            return comp_op_.apply(val, target_val_);
        }
        else {
            std::string str = ::wiserow::to_string(target_val_);
            boost::string_ref str_ref(str);
            return comp_op_.apply(val, str_ref);
        }
    }

    bool operator()(const std::complex<double>& val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;
        return comp_op_.apply(val, target_val_);
    }

private:
    const ComparisonOperator comp_op_;
    const T target_val_;
};

// -------------------------------------------------------------------------------------------------

template<>
class ComparisonVisitor<bool> : public BooleanVisitorDecorator
{
public:
    ComparisonVisitor(const BoolOp bool_op,
                      const CompOp comp_op,
                      const bool target_val,
                      const std::shared_ptr<BooleanVisitor>& visitor)
        : BooleanVisitorDecorator(bool_op, visitor, false)
        , comp_op_(comp_op)
        , target_val_(target_val)
    { }

    bool operator()(const int val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;
        return comp_op_.apply(val, static_cast<int>(target_val_));
    }

    bool operator()(const double val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;
        return comp_op_.apply(val, static_cast<int>(target_val_));
    }

    bool operator()(const boost::string_ref val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;
        std::string str = ::wiserow::to_string(target_val_);
        boost::string_ref str_ref(str);
        return comp_op_.apply(val, str_ref);
    }

    bool operator()(const std::complex<double>& val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;
        return comp_op_.apply(val, static_cast<int>(target_val_));
    }

private:
    const ComparisonOperator comp_op_;
    const bool target_val_;
};

} // namespace wiserow

#endif // WISEROW_BOOLEANVISITORS_H_
