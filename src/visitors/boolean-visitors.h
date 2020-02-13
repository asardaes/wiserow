#ifndef WISEROW_BOOLEANVISITORS_H_
#define WISEROW_BOOLEANVISITORS_H_

#include <complex>
#include <memory> // shared_ptr
#include <string>
#include <type_traits> // is_same
#include <unordered_set>
#include <vector>

#include <boost/utility/string_ref.hpp>

#include "../utils.h"
#include "BooleanVisitor.h"

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

// =================================================================================================
// Primary template will be for int and double

template<typename T>
class InSetVisitor : public BooleanVisitorDecorator
{
public:
    InSetVisitor(const BoolOp bool_op,
                 const std::shared_ptr<BooleanVisitor>& visitor,
                 const bool negate,
                 const T * const target_vals,
                 const std::size_t n_target_vals)
        : BooleanVisitorDecorator(bool_op, visitor, negate)
        , any_target_na_(false)
    {
        for (std::size_t i = 0; i < n_target_vals; i++) {
            if (na_visitor_(target_vals[i])) {
                any_target_na_ = true;
            }
            else {
                target_vals_.insert(target_vals[i]);
                target_string_vals_.insert(::wiserow::to_string(target_vals[i]));
            }
        }
    }

    bool operator()(const int val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;

        if (na_visitor_(val)) return forward(any_target_na_);

        return forward(target_vals_.find(val) != target_vals_.end());
    }

    bool operator()(const double val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;

        if (na_visitor_(val)) return forward(any_target_na_);
        // if set has integers but value is not whole, return false
        if (std::is_same<T, int>::value && val != static_cast<int>(val)) return forward(false);

        return forward(target_vals_.find(val) != target_vals_.end());
    }

    bool operator()(const boost::string_ref val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;
        if (na_visitor_(val)) return forward(any_target_na_);
        return forward(target_string_vals_.find(val.to_string()) != target_string_vals_.end());
    }

    bool operator()(const std::complex<double>& val) const override {
        bool super_ans = super(val);
        if (short_circuit(super_ans)) return super_ans;

        if (na_visitor_(val)) return forward(any_target_na_);
        if (val.imag() != 0) return forward(false);
        if (std::is_same<T, int>::value && val.real() != static_cast<int>(val.real())) return forward(false);

        return forward(target_vals_.find(val.real()) != target_vals_.end());
    }

private:
    bool any_target_na_;
    std::unordered_set<T> target_vals_;
    std::unordered_set<std::string> target_string_vals_;
    const NAVisitor na_visitor_;
};

// -------------------------------------------------------------------------------------------------

template<>
class InSetVisitor<std::complex<double>> : public BooleanVisitorDecorator
{
public:
    InSetVisitor(const BoolOp bool_op,
                 const std::shared_ptr<BooleanVisitor>& visitor,
                 const bool negate,
                 const std::complex<double> * const target_vals,
                 const std::size_t n_target_vals);

    bool operator()(const int val) const override;
    bool operator()(const double val) const override;
    bool operator()(const boost::string_ref val) const override;
    bool operator()(const std::complex<double>& val) const override;

private:
    bool any_target_na_;
    std::vector<std::complex<double>> target_vals_;
    std::unordered_set<std::string> target_string_vals_;
    const NAVisitor na_visitor_;
};

// -------------------------------------------------------------------------------------------------

template<>
class InSetVisitor<std::string> : public BooleanVisitorDecorator
{
public:
    InSetVisitor(const BoolOp bool_op,
                 const std::shared_ptr<BooleanVisitor>& visitor,
                 const bool negate,
                 std::unordered_set<std::string>&& target_vals,
                 const bool include_na);

    bool operator()(const int val) const override;
    bool operator()(const double val) const override;
    bool operator()(const boost::string_ref val) const override;
    bool operator()(const std::complex<double>& val) const override;

private:
    const std::unordered_set<std::string> target_vals_;
    const bool include_na_;
    const NAVisitor na_visitor_;
};

// =================================================================================================

class DuplicatedVisitor : public boost::static_visitor<bool>
{
public:
    bool operator()(const bool val);
    bool operator()(const int val);
    bool operator()(const double val);
    bool operator()(const std::complex<double>& val);
    bool operator()(const boost::string_ref val);

private:
    const NAVisitor na_visitor_;

    enum class Type {
        BOOL,
        INT,
        DOUBLE,
        COMPLEX,
        STRING
    };

    Type current_type_ = Type::BOOL;
    bool seen_na_ = false;

    std::unordered_set<bool> bools_;
    std::unordered_set<int> ints_;
    std::unordered_set<double> doubles_;
    std::vector<std::complex<double>> complexs_;
    std::unordered_set<std::string> strings_;

    void promote_to(Type type);
    bool handle_na();
};

} // namespace wiserow

#endif // WISEROW_BOOLEANVISITORS_H_
