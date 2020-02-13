#include "BooleanVisitor.h"

#include <unordered_set>
#include <utility> // move

#include <Rcpp.h>

#include "boolean-visitors.h"

namespace wiserow {

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

// =================================================================================================

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

BooleanVisitorBuilder::BooleanVisitorBuilder(const BoolOp op, const bool init)
    : op_(op)
    , visitor_(std::make_shared<InitBooleanVisitor>(init))
{ }

// -------------------------------------------------------------------------------------------------

BooleanVisitorBuilder& BooleanVisitorBuilder::is_na(const bool negate) {
    visitor_ = std::make_shared<NAVisitor>(op_, visitor_, negate);
    return *this;
}

// -------------------------------------------------------------------------------------------------

BooleanVisitorBuilder& BooleanVisitorBuilder::is_inf(const bool negate) {
    visitor_ = std::make_shared<InfiniteVisitor>(op_, visitor_, negate);
    return *this;
}

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

std::shared_ptr<BooleanVisitor> BooleanVisitorBuilder::build() {
    return visitor_;
}

} // namespace wiserow
