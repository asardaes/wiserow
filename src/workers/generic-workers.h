#ifndef WISEROW_GENERICWORKERS_H_
#define WISEROW_GENERICWORKERS_H_

#include <cstddef> // size_t
#include <memory>
#include <stdexcept> // logic_error
#include <string>
#include <vector>

#include <boost/utility/string_ref.hpp>
#include <boost/variant/get.hpp>
#include <Rcpp.h>

#include "../core.h"
#include "../utils.h"
#include "../visitors.h"
#include "worker-strategies.h"

namespace wiserow {

struct target_traits {
    bool is_na;
    char* char_target;
};

target_traits get_target_traits(const SEXP& target);

// =================================================================================================

template<typename T>
class CompBasedWorker : public ParallelWorker
{
public:
    CompBasedWorker(const OperationMetadata& metadata,
                    const ColumnCollection& cc,
                    OutputWrapper<T>& ans,
                    const SEXP& comp_op,
                    const Rcpp::List& target_vals,
                    const std::shared_ptr<OutputStrategy<T>>& out_strategy)
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

    virtual thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) override {
        bool any_na = false;
        std::shared_ptr<OutputStrategy<T>> thread_local_strategy =
            t_local ? std::static_pointer_cast<OutputStrategy<T>>(t_local) : out_strategy_->clone();

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


private:
    const NAVisitor na_visitor_;

    OutputWrapper<T>& ans_;
    const CompOp comp_op_;
    const std::shared_ptr<OutputStrategy<T>> out_strategy_;

    std::vector<std::shared_ptr<BooleanVisitor>> visitors_;
    std::vector<bool> na_targets_;

    // sigh, for case TRUE == "TRUE"
    std::vector<char *> char_targets_;
    const ComparisonOperator comp_operator_;
};

// =================================================================================================

template<typename T>
class RowArithWorker : public ParallelWorker
{
public:
    RowArithWorker(const OperationMetadata& metadata,
                   const ColumnCollection& cc,
                   OutputWrapper<T>& ans,
                   const Rcpp::List extras)
        : ParallelWorker(metadata, cc)
        , ans_(ans)
        , arith_opr_(parse_arith_op(Rcpp::as<std::string>(extras["arith_op"])))
    { }

    virtual thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr) override {
        bool need_init = arith_opr_.arith_op != ArithOp::ADD;
        T init;

        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            bool is_na = boost::apply_visitor(na_visitor_, col_collection_(in_id, j));

            if (is_na) {
                if (metadata.na_action == NaAction::PASS) {
                    ans_[out_id] = na_value_;
                    break;
                }
            }
            else if (need_init) {
                need_init = false;
                init = boost::apply_visitor(visitor_, col_collection_(in_id, j));
                ans_[out_id] = init;
            }
            else {
                const T variant = boost::apply_visitor(visitor_, col_collection_(in_id, j));
                ans_[out_id] = arith_opr_.apply(ans_[out_id], variant);
            }
        }

        return nullptr;
    }

private:
    OutputWrapper<T>& ans_;
    const ArithmeticOperator arith_opr_;

    const T na_value_ = NA_REAL;
    const NAVisitor na_visitor_;
    const NumericVisitor<T> visitor_;
};

// -------------------------------------------------------------------------------------------------
// Specialization for logical (which is integer in R) because any int > 1 is not really TRUE for R

template<>
class RowArithWorker<int> : public ParallelWorker
{
public:
    RowArithWorker(const OperationMetadata& metadata,
                   const ColumnCollection& cc,
                   OutputWrapper<int>& ans,
                   const Rcpp::List extras)
        : ParallelWorker(metadata, cc)
        , ans_(ans)
        , arith_opr_(parse_arith_op(Rcpp::as<std::string>(extras["arith_op"])))
    { }

    virtual thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr) override {
        bool need_init = arith_opr_.arith_op != ArithOp::ADD;
        int init;

        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            bool is_na = boost::apply_visitor(na_visitor_, col_collection_(in_id, j));

            if (is_na) {
                if (metadata.na_action == NaAction::PASS) {
                    ans_[out_id] = na_value_;
                    break;
                }
            }
            else if (need_init) {
                need_init = false;
                init = boost::apply_visitor(visitor_, col_collection_(in_id, j));
                ans_[out_id] = init;
            }
            else {
                const int variant = boost::apply_visitor(visitor_, col_collection_(in_id, j));
                ans_[out_id] = arith_opr_.apply(ans_[out_id], variant);
            }
        }

        int ans = ans_[out_id];
        if (metadata.output_mode == LGLSXP && ans != na_value_ && ans != 0) {
            ans_[out_id] = 1;
        }

        return nullptr;
    }

private:
    OutputWrapper<int>& ans_;
    const ArithmeticOperator arith_opr_;

    const int na_value_ = NA_INTEGER;
    const NAVisitor na_visitor_;
    const NumericVisitor<int> visitor_;
};

} // namespace wiserow

#endif // WISEROW_GENERICWORKERS_H_
