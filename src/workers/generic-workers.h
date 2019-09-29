#ifndef WISEROW_GENERICWORKERS_H_
#define WISEROW_GENERICWORKERS_H_

#include <cstddef> // size_t
#include <memory>
#include <string>

#include <Rcpp.h>

#include "../core.h"
#include "../utils.h"
#include "../visitors.h"

namespace wiserow {

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
        , cumulative_(Rcpp::as<bool>(extras["cumulative"]))
    { }

    virtual thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr) override {
        bool need_init = arith_opr_.arith_op != ArithOp::ADD;
        T init;

        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            bool is_na = boost::apply_visitor(na_visitor_, col_collection_(in_id, j));

            if (is_na) {
                if (metadata.na_action == NaAction::PASS) {
                    if (cumulative_) {
                        for (std::size_t k = j; k < col_collection_.ncol(); k++) {
                            ans_(out_id, k) = na_value_;
                        }
                    }
                    else {
                        ans_[out_id] = na_value_;
                    }

                    break;
                }
                else if (cumulative_) {
                    ans_(out_id, j) = ans_(out_id, j > 0 ? j - 1 : 0);
                }
            }
            else if (need_init) {
                need_init = false;
                init = boost::apply_visitor(visitor_, col_collection_(in_id, j));
                ans_(out_id, cumulative_ ? j : 0) = init;
            }
            else {
                const T variant = boost::apply_visitor(visitor_, col_collection_(in_id, j));
                std::size_t prev_j = cumulative_ ? (j > 0 ? j - 1 : 0) : 0;
                ans_(out_id, cumulative_ ? j : 0) = arith_opr_.apply(ans_(out_id, prev_j), variant);
            }
        }

        return nullptr;
    }

private:
    OutputWrapper<T>& ans_;
    const ArithmeticOperator arith_opr_;
    const bool cumulative_;

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
        , cumulative_(Rcpp::as<bool>(extras["cumulative"]))
    { }

    virtual thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr) override {
        bool need_init = arith_opr_.arith_op != ArithOp::ADD;
        int init;

        for (std::size_t j = 0; j < col_collection_.ncol(); j++) {
            bool is_na = boost::apply_visitor(na_visitor_, col_collection_(in_id, j));

            if (is_na) {
                if (metadata.na_action == NaAction::PASS) {
                    if (cumulative_) {
                        for (std::size_t k = j; k < col_collection_.ncol(); k++) {
                            ans_(out_id, k) = na_value_;
                        }
                    }
                    else {
                        ans_[out_id] = na_value_;
                    }

                    break;
                }
                else if (cumulative_) {
                    ans_(out_id, j) = ans_(out_id, j > 0 ? j - 1 : 0);
                }
            }
            else if (need_init) {
                need_init = false;
                init = boost::apply_visitor(visitor_, col_collection_(in_id, j));
                ans_(out_id, cumulative_ ? j : 0) = init;
            }
            else {
                const int variant = boost::apply_visitor(visitor_, col_collection_(in_id, j));
                std::size_t prev_j = cumulative_ ? (j > 0 ? j - 1 : 0) : 0;
                ans_(out_id, cumulative_ ? j : 0) = arith_opr_.apply(ans_(out_id, prev_j), variant);
            }
        }

        std::size_t max_j = cumulative_ ? col_collection_.ncol() : 1;
        for (std::size_t j = 0; j < max_j; j++) {
            int ans = ans_(out_id, j);
            if (metadata.output_mode == LGLSXP && ans != na_value_ && ans != 0) {
                ans_(out_id, j) = 1;
            }
        }

        return nullptr;
    }

private:
    OutputWrapper<int>& ans_;
    const ArithmeticOperator arith_opr_;
    const bool cumulative_;

    const int na_value_ = NA_INTEGER;
    const NAVisitor na_visitor_;
    const NumericVisitor<int> visitor_;
};

} // namespace wiserow

#endif // WISEROW_GENERICWORKERS_H_
