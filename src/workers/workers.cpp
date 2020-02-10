#include "../workers.h"

#include <complex>
#include <memory>
#include <stdexcept> // logic_error
#include <string>
#include <typeinfo>

#include <boost/utility/string_ref.hpp>
#include <boost/variant/get.hpp>

#include <Rcpp.h>

namespace wiserow {

boost::string_ref RowExtremaWorker<boost::string_ref, false>::STRING_REF_NOT_SET = boost::string_ref("dummy");

RowExtremaWorker<boost::string_ref, false>::RowExtremaWorker(const OperationMetadata& metadata,
                                                      const ColumnCollection& cc,
                                                      const Rcpp::List extras,
                                                      std::unordered_set<std::string>& temporary_strings)
    : ParallelWorker(metadata, cc)
    , ans(cc.nrow())
    , comp_op_(parse_comp_op(Rcpp::as<std::string>(extras["comp_op"])))
    , dummy_parent_visitor_(std::make_shared<InitBooleanVisitor>(true))
    , temporary_strings_(temporary_strings)
{ }

// -------------------------------------------------------------------------------------------------

ParallelWorker::thread_local_ptr RowExtremaWorker<boost::string_ref, false>::work_row(std::size_t in_id,
                                                                                      std::size_t out_id,
                                                                                      ParallelWorker::thread_local_ptr t_local)
{
    supported_col_t variant;
    bool variant_initialized = false;
    std::shared_ptr<BooleanVisitor> visitor = nullptr;

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        const supported_col_t& next_variant = col_collection_(in_id, j);
        bool is_na = boost::apply_visitor(na_visitor_, next_variant);

        if (is_na) {
            if (metadata.na_action == NaAction::EXCLUDE) {
                continue;
            }
            else {
                variant = STRING_REF_NOT_SET;
                variant_initialized = true;
                break;
            }
        }
        else if (!visitor) {
            variant = coerce(next_variant, col_collection_[j]->is_logical());
            visitor = instantiate_visitor(boost::get<boost::string_ref>(variant));
        }
        else {
            const supported_col_t coerced_next = coerce(next_variant, col_collection_[j]->is_logical());
            bool next_more_extreme = boost::apply_visitor(*visitor, coerced_next);
            if (next_more_extreme) {
                variant = coerced_next;
                visitor = instantiate_visitor(boost::get<boost::string_ref>(variant));
            }
        }

        variant_initialized = true;
    }

    ans[out_id] = variant_initialized ? boost::get<boost::string_ref>(variant) : STRING_REF_NOT_SET;
    return nullptr;
}

// -------------------------------------------------------------------------------------------------

std::shared_ptr<BooleanVisitor> RowExtremaWorker<boost::string_ref, false>::instantiate_visitor(const boost::string_ref& str_ref) {
    return std::make_shared<ComparisonVisitor<boost::string_ref>>(bool_op_,
                                                                  comp_op_,
                                                                  str_ref,
                                                                  dummy_parent_visitor_);
}

// -------------------------------------------------------------------------------------------------

boost::string_ref RowExtremaWorker<boost::string_ref, false>::coerce(const supported_col_t& variant, const bool is_logical) {
    std::string val;

    if (variant.type() == typeid(int)) {
        val = is_logical ? ::wiserow::to_string(boost::get<int>(variant) != 0) :  ::wiserow::to_string(boost::get<int>(variant));
    }
    else if (variant.type() == typeid(double)) {
        val = ::wiserow::to_string(boost::get<double>(variant));
    }
    else if (variant.type() == typeid(boost::string_ref)) {
        return boost::get<boost::string_ref>(variant);
    }
    else {
        throw std::runtime_error("[wiserow] Invalid type passed to RowExtremaWorker. This should not happen."); // nocov
    }

    this->mutex_.lock();
    auto it = temporary_strings_.insert(std::move(val));
    this->mutex_.unlock();
    return boost::string_ref(it.first->c_str());
}

// =================================================================================================

BoolTestWorker::BoolTestWorker(const OperationMetadata& metadata,
                               const ColumnCollection& cc,
                               OutputWrapper<int>& ans,
                               const std::shared_ptr<BooleanVisitor>& visitor,
                               const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , visitor_(visitor)
    , out_strategy_(out_strategy)
{ }

// -------------------------------------------------------------------------------------------------

ParallelWorker::thread_local_ptr BoolTestWorker::work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) {
    std::shared_ptr<OutputStrategy<int>> thread_local_strategy =
            t_local ? std::static_pointer_cast<OutputStrategy<int>>(t_local) : out_strategy_->clone();

    thread_local_strategy->reinit();

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        supported_col_t variant = col_collection_(in_id, j);
        thread_local_strategy->apply(j, variant, boost::apply_visitor(*visitor_, variant));

        if (thread_local_strategy->short_circuit()) {
            break;
        }
    }

    ans_[out_id] = thread_local_strategy->output(metadata, col_collection_.ncol(), false);
    return thread_local_strategy;
}

// =================================================================================================

NATestWorker::NATestWorker(const OperationMetadata& metadata,
                           const ColumnCollection& cc,
                           OutputWrapper<int>& ans,
                           const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : BoolTestWorker(metadata, cc, ans, BooleanVisitorBuilder().is_na().build(), out_strategy)
{ }

// =================================================================================================

InfTestWorker::InfTestWorker(const OperationMetadata& metadata,
                             const ColumnCollection& cc,
                             OutputWrapper<int>& ans,
                             const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : BoolTestWorker(metadata, cc, ans, BooleanVisitorBuilder().is_inf().build(), out_strategy)
{ }

// =================================================================================================

FiniteTestWorker::FiniteTestWorker(const OperationMetadata& metadata,
                                   const ColumnCollection& cc,
                                   OutputWrapper<int>& ans,
                                   const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : BoolTestWorker(metadata, cc, ans, BooleanVisitorBuilder(BoolOp::AND, true).is_na(true).is_inf(true).build(), out_strategy)
{ }

// =================================================================================================

struct target_traits {
    bool is_na;
    char* char_target;
};

target_traits get_target_traits(const SEXP& target) {
    switch(TYPEOF(target)) {
    case INTSXP:
        return { Rcpp::traits::is_na<INTSXP>(Rcpp::as<int>(target)), nullptr };
    case REALSXP:
        return { Rcpp::traits::is_na<REALSXP>(Rcpp::as<double>(target)), nullptr };
    case LGLSXP:
        return { Rcpp::traits::is_na<LGLSXP>(Rcpp::as<int>(target)), nullptr };
    case CPLXSXP:
        return { Rcpp::traits::is_na<CPLXSXP>(Rcpp::as<Rcomplex>(target)), nullptr };
    case STRSXP: {
        Rcpp::StringVector vec(target);

        if (Rcpp::traits::is_na<STRSXP>(vec[0])) {
            return { true, nullptr };
        }
        else {
            return { false, vec[0] };
        }
    }
    default:
        Rcpp::stop("[wiserow] target values for comparison can only be integers, doubles, logicals, complex numbers, or strings.");
    }
}

// -------------------------------------------------------------------------------------------------

CompBasedWorker::CompBasedWorker(const OperationMetadata& metadata,
                                 const ColumnCollection& cc,
                                 OutputWrapper<int>& ans,
                                 const SEXP& comp_op,
                                 const Rcpp::List& target_vals,
                                 const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , comp_op_(parse_comp_op(Rcpp::as<std::string>(comp_op)))
    , out_strategy_(out_strategy)
    , comp_operator_(comp_op_)
{
    if (!out_strategy_) { // nocov start
        throw std::logic_error("Output strategy cannot be null.");
    } // nocov end

    for (R_xlen_t i = 0; i < target_vals.length(); i++) {
        target_traits tt = get_target_traits(target_vals[i]);
        visitors_.push_back(BooleanVisitorBuilder().compare(comp_op_, target_vals[i]).build());
        na_targets_.push_back(tt.is_na);
        char_targets_.push_back(tt.char_target);
    }
}

ParallelWorker::thread_local_ptr CompBasedWorker::work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) {
    bool any_na = false;
    std::shared_ptr<OutputStrategy<int>> thread_local_strategy =
        t_local ? std::static_pointer_cast<OutputStrategy<int>>(t_local) : out_strategy_->clone();

    thread_local_strategy->reinit();

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        auto visitor = visitors_[j % visitors_.size()];
        bool na_target = na_targets_[j % na_targets_.size()];
        const char *char_target = char_targets_[j % char_targets_.size()];
        supported_col_t variant = col_collection_(in_id, j);

        if (!na_target) {
            bool is_na = boost::apply_visitor(na_visitor_, variant);
            if (is_na) {
                if (metadata.na_action == NaAction::PASS) any_na = true;
                continue;
            }
        }
        else if (comp_op_ != CompOp::EQ && comp_op_ != CompOp::NEQ) {
            // if target for comparison is NA but operator is not one of [==, !=], result is NA
            any_na = true;
            continue;
        }

        if (char_target && col_collection_[j]->is_logical()) {
            // tricky case when source is R-logical (with underlying int) that should be converted to string
            bool variant_bool = boost::get<int>(variant) != 0;
            boost::string_ref str_ref(char_target);
            thread_local_strategy->apply(j, variant, comp_operator_.apply(variant_bool, str_ref));
        }
        else {
            thread_local_strategy->apply(j, variant, boost::apply_visitor(*visitor, variant));
        }

        if (thread_local_strategy->short_circuit()) {
            break;
        }
    }

    ans_[out_id] = thread_local_strategy->output(metadata, col_collection_.ncol(), any_na);
    return thread_local_strategy;
}

// =================================================================================================

InSetWorker::InSetWorker(const OperationMetadata& metadata,
                         const ColumnCollection& cc,
                         OutputWrapper<int>& ans,
                         const Rcpp::List& target_sets,
                         const bool negate,
                         const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , out_strategy_(out_strategy)
{
    if (!out_strategy_) { // nocov start
        throw std::logic_error("Output strategy cannot be null.");
    } // nocov end

    for (R_xlen_t i = 0; i < target_sets.length(); i++) {
        visitors_.push_back(BooleanVisitorBuilder().in_set(target_sets[i], negate).build());
        char_targets_.push_back(TYPEOF(target_sets[i]) == STRSXP);
    }
}

ParallelWorker::thread_local_ptr InSetWorker::work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) {
    std::shared_ptr<OutputStrategy<int>> thread_local_strategy =
        t_local ? std::static_pointer_cast<OutputStrategy<int>>(t_local) : out_strategy_->clone();

    thread_local_strategy->reinit();

    for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
        auto visitor = visitors_[j % visitors_.size()];
        supported_col_t variant = col_collection_(in_id, j);

        if (char_targets_[j % char_targets_.size()] && col_collection_[j]->is_logical()) {
            // tricky case when source is R-logical (with underlying int) that should be converted to string
            int variant_int = boost::get<int>(variant);
            if (variant_int == NA_INTEGER) {
                thread_local_strategy->apply(j, variant, boost::apply_visitor(*visitor, variant));
            }
            else {
                const char *int_char = variant_int == 0 ? "FALSE" : "TRUE";
                boost::string_ref str_ref(int_char);
                thread_local_strategy->apply(j, variant, (*visitor)(str_ref));
            }
        }
        else {
            thread_local_strategy->apply(j, variant, boost::apply_visitor(*visitor, variant));
        }

        if (thread_local_strategy->short_circuit()) {
            break;
        }
    }

    ans_[out_id] = thread_local_strategy->output(metadata, col_collection_.ncol(), false);
    return thread_local_strategy;
}

// =================================================================================================

DuplicatedWorker::DuplicatedWorker(const OperationMetadata& metadata,
                                   const ColumnCollection& cc,
                                   OutputWrapper<int>& ans,
                                   const std::shared_ptr<OutputStrategy<int>>& out_strategy)
    : ParallelWorker(metadata, cc)
    , ans_(ans)
    , out_strategy_(out_strategy)
{ }

ParallelWorker::thread_local_ptr DuplicatedWorker::work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) {
    std::shared_ptr<OutputStrategy<int>> thread_local_strategy =
            t_local ? std::static_pointer_cast<OutputStrategy<int>>(t_local) : out_strategy_->clone();

    DuplicatedVisitor duplicated_visitor;

    if (thread_local_strategy) {
        thread_local_strategy->reinit();

        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            supported_col_t variant = col_collection_(in_id, j);

            if (col_collection_[j]->is_logical()) {
                int variant_int = boost::get<int>(variant);

                if (variant_int == NA_INTEGER) {
                    thread_local_strategy->apply(j, variant, boost::apply_visitor(duplicated_visitor, variant));
                }
                else {
                    bool int_bool = static_cast<bool>(variant_int);
                    thread_local_strategy->apply(j, variant, (duplicated_visitor)(int_bool));
                }
            }
            else {
                thread_local_strategy->apply(j, variant, boost::apply_visitor(duplicated_visitor, variant));
            }

            if (thread_local_strategy->short_circuit()) {
                break;
            }
        }

        ans_[out_id] = thread_local_strategy->output(metadata, col_collection_.ncol(), false);
        return thread_local_strategy;
    }
    else {
        // thread_local_strategy is null -> IdentityStrategy
        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            supported_col_t variant = col_collection_(in_id, j);

            if (col_collection_[j]->is_logical()) {
                int variant_int = boost::get<int>(variant);

                if (variant_int == NA_INTEGER) {
                    ans_(out_id, j) = boost::apply_visitor(duplicated_visitor, variant);
                }
                else {
                    bool int_bool = static_cast<bool>(variant_int);
                    ans_(out_id, j) = (duplicated_visitor)(int_bool);
                }
            }
            else {
                ans_(out_id, j) = boost::apply_visitor(duplicated_visitor, variant);
            }
        }

        return nullptr;
    }
}

// =================================================================================================

// nocov start
void IdentityStrategy::apply(const std::size_t, const supported_col_t&, const bool) {
    throw std::logic_error("IdentityStrategy's apply() should not be called.");
}

int IdentityStrategy::output(const OperationMetadata&, const std::size_t, const bool) {
    throw std::logic_error("IdentityStrategy's output() should not be called.");
}
// nocov end

std::shared_ptr<OutputStrategy<int>> IdentityStrategy::clone() {
    return nullptr;
}

// =================================================================================================

BulkBoolStrategy::BulkBoolStrategy(const BulkBoolOp& bb_op, const NaAction& na_action)
    : bb_op_(bb_op)
    , logical_operator_(bb_op == BulkBoolOp::ALL ? BoolOp::AND : BoolOp::OR)
    , na_action_(na_action)
    , init_(bb_op == BulkBoolOp::ALL ? true : false)
    , flag_(init_)
{ }

void BulkBoolStrategy::reinit() {
    flag_ = init_;
}

bool BulkBoolStrategy::short_circuit() {
    // maybe don't break because R's all/any still check all values for NA when na.rm = FALSE
    if (na_action_ == NaAction::EXCLUDE) {
        if (bb_op_ == BulkBoolOp::ALL && !flag_) {
            return true;
        }
        else if (flag_ && (bb_op_ == BulkBoolOp::ANY || bb_op_ == BulkBoolOp::NONE)) {
            return true;
        }
    }

    return false;
}

void BulkBoolStrategy::apply(const std::size_t, const supported_col_t&, const bool match_flag) {
    flag_ = logical_operator_.apply(flag_, match_flag);
}

int BulkBoolStrategy::output(const OperationMetadata&, const std::size_t ncol, const bool any_na) {
    switch(bb_op_) {
    case BulkBoolOp::ALL: {
        if (ncol > 0) {
            if (flag_ && any_na) {
                return NA_INTEGER;
            }
            else {
                return flag_;
            }
        }
        else {
            return 0;
        }
    }
    case BulkBoolOp::ANY: {
        if (!flag_ && any_na) {
            return NA_INTEGER;
        }
        else {
            return flag_;
        }
    }
    case BulkBoolOp::NONE: {
        if (!flag_ && any_na) {
            return NA_INTEGER;
        }
        else {
            return !flag_;
        }
    }
    }

    return NA_INTEGER; // nocov
}

std::shared_ptr<OutputStrategy<int>> BulkBoolStrategy::clone() {
    return std::make_shared<BulkBoolStrategy>(this->bb_op_, this->na_action_);
}

// =================================================================================================

WhichFirstStrategy::WhichFirstStrategy()
    : which_(-1)
{ }

void WhichFirstStrategy::reinit() {
    which_ = -1;
}

bool WhichFirstStrategy::short_circuit() {
    return which_ >= 0;
}

void WhichFirstStrategy::apply(const std::size_t col, const supported_col_t&, const bool match_flag) {
    if (match_flag) {
        which_ = col;
    }
}

int WhichFirstStrategy::output(const OperationMetadata& metadata, const std::size_t, const bool) {
    if (which_ < 0) {
        return NA_INTEGER;
    }
    /*else if (metadata.cols.ptr) {
        return metadata.cols.ptr[which_]; TODO
    }*/
    else {
        return static_cast<int>(which_ + 1);
    }
}

std::shared_ptr<OutputStrategy<int>> WhichFirstStrategy::clone() {
    return std::make_shared<WhichFirstStrategy>();
}

// =================================================================================================

CountStrategy::CountStrategy()
    : count_(0)
{ }

void CountStrategy::reinit() {
    count_ = 0;
}

void CountStrategy::apply(const std::size_t, const supported_col_t&, const bool match_flag) {
    if (match_flag) {
        count_++;
    }
}

int CountStrategy::output(const OperationMetadata&, const std::size_t, const bool any_na) {
    if (any_na) {
        return NA_INTEGER;
    }
    else {
        return count_;
    }
}

std::shared_ptr<OutputStrategy<int>> CountStrategy::clone() {
    return std::make_shared<CountStrategy>();
}

} // namespace wiserow
