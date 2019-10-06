#include "../visitors.h"

#include <algorithm> // find
#include <cmath> // isfinite
#include <unordered_set>
#include <utility> // move

#include <Rcpp.h>

#include "boolean-visitors.h"

namespace wiserow {

// Singleton, if address is same, string is NA
static const char *na_string_ptr = CHAR(Rf_asChar(NA_STRING));

// =================================================================================================

BooleanVisitorBuilder::BooleanVisitorBuilder(const BoolOp op, const bool init)
    : op_(op)
    , visitor_(std::make_shared<InitBooleanVisitor>(init))
{ }

BooleanVisitorBuilder& BooleanVisitorBuilder::is_na(const bool negate) {
    visitor_ = std::make_shared<NAVisitor>(op_, visitor_, negate);
    return *this;
}

BooleanVisitorBuilder& BooleanVisitorBuilder::is_inf(const bool negate) {
    visitor_ = std::make_shared<InfiniteVisitor>(op_, visitor_, negate);
    return *this;
}

BooleanVisitorBuilder& BooleanVisitorBuilder::compare(const CompOp comp_op, const SEXP& target_val) {
    bool negate = comp_op == CompOp::EQ ? false : true;

    switch(TYPEOF(target_val)) {
    case INTSXP: {
        Rcpp::IntegerVector vec(target_val);
        int val = vec[0];

        if (Rcpp::traits::is_na<INTSXP>(val)) {
            visitor_ = std::make_shared<NAVisitor>(op_, visitor_, negate);
        }
        else {
            visitor_ = std::make_shared<ComparisonVisitor<int>>(op_, comp_op, val, visitor_);
        }

        break;
    }
    case REALSXP: {
        Rcpp::NumericVector vec(target_val);
        double val = vec[0];

        if (Rcpp::traits::is_na<REALSXP>(val)) {
            visitor_ = std::make_shared<NAVisitor>(op_, visitor_, negate);
        }
        else {
            visitor_ = std::make_shared<ComparisonVisitor<double>>(op_, comp_op, val, visitor_);
        }

        break;
    }
    case LGLSXP: {
        Rcpp::LogicalVector vec(target_val);
        int val = vec[0];

        if (Rcpp::traits::is_na<LGLSXP>(val)) {
            visitor_ = std::make_shared<NAVisitor>(op_, visitor_, negate);
        }
        else {
            visitor_ = std::make_shared<ComparisonVisitor<bool>>(op_, comp_op, val != 0, visitor_);
        }

        break;
    }
    case CPLXSXP: {
        Rcpp::ComplexVector vec(target_val);
        Rcomplex val = vec[0];

        if (Rcpp::traits::is_na<CPLXSXP>(val)) {
            visitor_ = std::make_shared<NAVisitor>(op_, visitor_, negate);
        }
        else {
            std::complex<double> cplx_val(val.r, val.i);
            visitor_ = std::make_shared<ComparisonVisitor<std::complex<double>>>(op_, comp_op, cplx_val, visitor_);
        }

        break;
    }
    case STRSXP: {
        Rcpp::StringVector vec(target_val);

        if (Rcpp::traits::is_na<STRSXP>(vec[0])) {
            visitor_ = std::make_shared<NAVisitor>(op_, visitor_, negate);
        }
        else {
            // https://stackoverflow.com/a/7875438/5793905
            Rcpp::CharacterVector val = Rcpp::as<Rcpp::CharacterVector>(vec[0]);
            const char *val_ptr = static_cast<char *>(val[0]);
            boost::string_ref str_ref(val_ptr);
            visitor_ = std::make_shared<ComparisonVisitor<boost::string_ref>>(op_, comp_op, str_ref, visitor_);
        }

        break;
    }
    }

    return *this;
}

BooleanVisitorBuilder& BooleanVisitorBuilder::in_set(const SEXP& target_vals, const bool negate) {
    switch(TYPEOF(target_vals)) {
    case INTSXP: {
        Rcpp::IntegerVector vec(target_vals);
        if (vec.size() > 0) {
            visitor_ = std::make_shared<InSetVisitor<int>>(op_, visitor_, negate, &vec[0], vec.size());
        }
        break;
    }
    case REALSXP: {
        Rcpp::NumericVector vec(target_vals);
        if (vec.size() > 0) {
            visitor_ = std::make_shared<InSetVisitor<double>>(op_, visitor_, negate, &vec[0], vec.size());
        }
        break;
    }
    case LGLSXP: {
        Rcpp::LogicalVector vec(target_vals);
        if (vec.size() > 0) {
            visitor_ = std::make_shared<InSetVisitor<int>>(op_, visitor_, negate, &vec[0], vec.size());
        }
        break;
    }
    case CPLXSXP: {
        Rcpp::ComplexVector vec(target_vals);
        if (vec.size() > 0) {
            visitor_ = std::make_shared<InSetVisitor<std::complex<double>>>(
                op_, visitor_, negate, reinterpret_cast<std::complex<double> *>(&vec[0]), vec.size());
        }
        break;
    }
    case STRSXP: {
        Rcpp::StringVector vec(target_vals);
        if (vec.size() > 0) {
            std::unordered_set<std::string> set;
            bool any_na = false;

            for (R_xlen_t i = 0; i < vec.size(); i++) {
                if (Rcpp::traits::is_na<STRSXP>(vec[i])) {
                    if (!any_na) any_na = true;
                }
                else {
                    Rcpp::CharacterVector val = Rcpp::as<Rcpp::CharacterVector>(vec[i]);
                    const char *val_ptr = static_cast<char *>(val[0]);
                    set.insert(std::string(val_ptr));
                }
            }

            visitor_ = std::make_shared<InSetVisitor<std::string>>(op_, visitor_, negate, std::move(set), any_na);
        }
        break;
    }
    }

    return *this;
}

std::shared_ptr<BooleanVisitor> BooleanVisitorBuilder::build() {
    return visitor_;
}

// =================================================================================================

InitBooleanVisitor::InitBooleanVisitor(const bool init)
    : init_(init)
{ }

bool InitBooleanVisitor::operator()(const int val) const {
    return init_;
}

bool InitBooleanVisitor::operator()(const double val) const {
    return init_;
}

bool InitBooleanVisitor::operator()(const boost::string_ref val) const {
    return init_;
}

bool InitBooleanVisitor::operator()(const std::complex<double>& val) const {
    return init_;
}

// -------------------------------------------------------------------------------------------------

BooleanVisitorDecorator::BooleanVisitorDecorator(const BoolOp op,
                                                 const std::shared_ptr<BooleanVisitor>& visitor,
                                                 const bool negate)
    : op_(op)
    , parent_(visitor)
    , negate_(negate)
{ }

bool BooleanVisitorDecorator::short_circuit(const bool val) const {
    if ((op_ == BoolOp::AND && !val) || (op_ == BoolOp::OR && val)) {
        return true;
    }
    else {
        return false;
    }
}

// if this is called, it means short_circuit was already tested
bool BooleanVisitorDecorator::forward(const bool ans) const {
    return ans ^ negate_; // XOR
}

// =================================================================================================
// NAVisitor

NAVisitor::NAVisitor()
    : BooleanVisitorDecorator(BoolOp::OR, std::make_shared<InitBooleanVisitor>(false), false)
{ }

NAVisitor::NAVisitor(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor, const bool negate)
    : BooleanVisitorDecorator(op, visitor, negate)
{ }

bool NAVisitor::operator()(const int val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(Rcpp::IntegerVector::is_na(val));
}

bool NAVisitor::operator()(const double val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(Rcpp::NumericVector::is_na(val));
}

bool NAVisitor::operator()(const boost::string_ref val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(val.data() == na_string_ptr);
}

bool NAVisitor::operator()(const std::complex<double>& val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(Rcpp::NumericVector::is_na(val.real()) || Rcpp::NumericVector::is_na(val.imag()));
}

// =================================================================================================
// InfiniteVisitor

InfiniteVisitor::InfiniteVisitor(const BoolOp op, const std::shared_ptr<BooleanVisitor>& visitor, const bool negate)
    : BooleanVisitorDecorator(op, visitor, negate)
{ }

bool InfiniteVisitor::operator()(const int val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(false);
}

bool InfiniteVisitor::operator()(const double val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(!na_visitor_(val) && !std::isfinite(val));
}

bool InfiniteVisitor::operator()(const boost::string_ref val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return false; // corner case, R says strings are neither infinite nor finite...
}

bool InfiniteVisitor::operator()(const std::complex<double>& val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    return forward(!na_visitor_(val) && (!std::isfinite(val.real()) || !std::isfinite(val.imag())));
}

// =================================================================================================

InSetVisitor<std::complex<double>>::InSetVisitor(const BoolOp bool_op,
                                                 const std::shared_ptr<BooleanVisitor>& visitor,
                                                 const bool negate,
                                                 const std::complex<double> * const target_vals,
                                                 const std::size_t n_target_vals)
    : BooleanVisitorDecorator(bool_op, visitor, negate)
    , any_target_na_(false)
{
    for (std::size_t i = 0; i < n_target_vals; i++) {
        if (na_visitor_(target_vals[i])) {
            any_target_na_ = true;
        }
        else {
            target_vals_.push_back(target_vals[i]);
            target_string_vals_.insert(::wiserow::to_string(target_vals[i]));
        }
    }
}

bool InSetVisitor<std::complex<double>>::operator()(const int val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    if (na_visitor_(val)) return forward(any_target_na_);
    return forward(std::find(target_vals_.begin(), target_vals_.end(), std::complex<double>(val, 0)) != target_vals_.end());
}

bool InSetVisitor<std::complex<double>>::operator()(const double val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    if (na_visitor_(val)) return forward(any_target_na_);
    return forward(std::find(target_vals_.begin(), target_vals_.end(), std::complex<double>(val, 0)) != target_vals_.end());
}

bool InSetVisitor<std::complex<double>>::operator()(const boost::string_ref val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    if (na_visitor_(val)) return forward(any_target_na_);
    return forward(target_string_vals_.find(val.to_string()) != target_string_vals_.end());
}

bool InSetVisitor<std::complex<double>>::operator()(const std::complex<double>& val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;
    if (na_visitor_(val)) return forward(any_target_na_);
    return forward(std::find(target_vals_.begin(), target_vals_.end(), val) != target_vals_.end());
}

// -------------------------------------------------------------------------------------------------

InSetVisitor<std::string>::InSetVisitor(const BoolOp bool_op,
                                        const std::shared_ptr<BooleanVisitor>& visitor,
                                        const bool negate,
                                        std::unordered_set<std::string>&& target_vals,
                                        const bool include_na)
    : BooleanVisitorDecorator(bool_op, visitor, negate)
    , target_vals_(target_vals)
    , include_na_(include_na)
{ }

bool InSetVisitor<std::string>::operator()(const int val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;

    if (na_visitor_(val)) return forward(include_na_);

    std::string str_val = ::wiserow::to_string(val);
    return forward(target_vals_.find(str_val) != target_vals_.end());
}

bool InSetVisitor<std::string>::operator()(const double val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;

    if (na_visitor_(val)) return forward(include_na_);

    std::string str_val = ::wiserow::to_string(val);
    return forward(target_vals_.find(str_val) != target_vals_.end());
}

bool InSetVisitor<std::string>::operator()(const boost::string_ref val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;

    if (na_visitor_(val)) return forward(include_na_);
    return forward(target_vals_.find(val.to_string()) != target_vals_.end());
}

bool InSetVisitor<std::string>::operator()(const std::complex<double>& val) const {
    bool super_ans = super(val);
    if (short_circuit(super_ans)) return super_ans;

    if (na_visitor_(val)) return forward(include_na_);

    std::string str_val = ::wiserow::to_string(val);
    return forward(target_vals_.find(str_val) != target_vals_.end());
}

// =================================================================================================
// TODO: maybe call promote_to before/after na_visitor depending on na_action

bool DuplicatedVisitor::operator()(const bool val) {
    switch(current_type_) {
    case Type::BOOL:
        return !bools_.insert(val).second;
    case Type::INT:
        return !ints_.insert(static_cast<int>(val)).second;
    case Type::DOUBLE:
        return !doubles_.insert(static_cast<double>(val)).second;
    case Type::COMPLEX: {
        std::complex<double> cplx(static_cast<double>(val), 0);
        bool ans = std::find(complexs_.begin(), complexs_.end(), cplx) != complexs_.end();
        if (!ans) complexs_.push_back(cplx);
        return ans;
    }
    case Type::STRING:
        return !strings_.insert(::wiserow::to_string(val)).second;
    }

    throw "Unreachable code reached..."; // nocov
}

bool DuplicatedVisitor::operator()(const int val) {
    if (na_visitor_(val)) return handle_na();

    promote_to(Type::INT);

    switch(current_type_) {
    case Type::BOOL:
    case Type::INT:
        return !ints_.insert(val).second;
    case Type::DOUBLE:
        return !doubles_.insert(static_cast<double>(val)).second;
    case Type::COMPLEX: {
        std::complex<double> cplx(val, 0);
        bool ans = std::find(complexs_.begin(), complexs_.end(), cplx) != complexs_.end();
        if (!ans) complexs_.push_back(cplx);
        return ans;
    }
    case Type::STRING:
        return !strings_.insert(::wiserow::to_string(val)).second;
    }

    throw "Unreachable code reached..."; // nocov
}

bool DuplicatedVisitor::operator()(const double val) {
    if (na_visitor_(val)) return handle_na();

    promote_to(Type::DOUBLE);

    switch(current_type_) {
    case Type::BOOL:
    case Type::INT:
    case Type::DOUBLE:
        return !doubles_.insert(static_cast<double>(val)).second;
    case Type::COMPLEX: {
        std::complex<double> cplx(val, 0);
        bool ans = std::find(complexs_.begin(), complexs_.end(), cplx) != complexs_.end();
        if (!ans) complexs_.push_back(cplx);
        return ans;
    }
    case Type::STRING:
        return !strings_.insert(::wiserow::to_string(val)).second;
    }

    throw "Unreachable code reached..."; // nocov
}

bool DuplicatedVisitor::operator()(const std::complex<double>& val) {
    if (na_visitor_(val)) return handle_na();

    promote_to(Type::COMPLEX);

    switch(current_type_) {
    case Type::BOOL:
    case Type::INT:
    case Type::DOUBLE:
    case Type::COMPLEX: {
        bool ans = std::find(complexs_.begin(), complexs_.end(), val) != complexs_.end();
        if (!ans) complexs_.push_back(val);
        return ans;
    }
    case Type::STRING:
        return !strings_.insert(::wiserow::to_string(val)).second;
    }

    throw "Unreachable code reached..."; // nocov
}

bool DuplicatedVisitor::operator()(const boost::string_ref val) {
    if (na_visitor_(val)) return handle_na();

    promote_to(Type::STRING);

    switch(current_type_) {
    case Type::BOOL:
    case Type::INT:
    case Type::DOUBLE:
    case Type::COMPLEX:
    case Type::STRING:
        return !strings_.insert(val.to_string()).second;
    }

    throw "Unreachable code reached..."; // nocov
}

void DuplicatedVisitor::promote_to(Type type) {
    switch(type) {
    case Type::BOOL:
    case Type::INT:
        if (current_type_ >= Type::INT) break;
        ints_.insert(bools_.begin(), bools_.end());
        current_type_ = Type::INT;
        break;
    case Type::DOUBLE:
        if (current_type_ >= Type::DOUBLE) break;

        if (current_type_ == Type::BOOL) {
            doubles_.insert(bools_.begin(), bools_.end());
        }
        else if (current_type_ == Type::INT) {
            doubles_.insert(ints_.begin(), ints_.end());
        }

        current_type_ = Type::DOUBLE;
        break;
    case Type::COMPLEX:
        if (current_type_ >= Type::COMPLEX) break;

        if (current_type_ == Type::BOOL) {
            for (bool val : bools_) {
                complexs_.push_back(std::complex<double>(val, 0));
            }
        }
        else if (current_type_ == Type::INT) {
            for (int val : ints_) {
                complexs_.push_back(std::complex<double>(val, 0));
            }
        }
        else if (current_type_ == Type::DOUBLE) {
            for (double val : doubles_) {
                complexs_.push_back(std::complex<double>(val, 0));
            }
        }

        current_type_ = Type::COMPLEX;
        break;
    case Type::STRING:
        if (current_type_ >= Type::STRING) break;

        if (current_type_ == Type::BOOL) {
            for (bool val : bools_) {
                strings_.insert(::wiserow::to_string(val));
            }
        }
        else if (current_type_ == Type::INT) {
            for (int val : ints_) {
                strings_.insert(::wiserow::to_string(val));
            }
        }
        else if (current_type_ == Type::DOUBLE) {
            for (double val : doubles_) {
                strings_.insert(::wiserow::to_string(val));
            }
        }
        else if (current_type_ == Type::COMPLEX) {
            for (std::complex<double>& val : complexs_) {
                strings_.insert(::wiserow::to_string(val));
            }
        }

        current_type_ = Type::STRING;
        break;
    }
}

bool DuplicatedVisitor::handle_na() {
    if (seen_na_) {
        return true;
    }
    else {
        seen_na_ = true;
        return false;
    }
}

} // namespace wiserow
