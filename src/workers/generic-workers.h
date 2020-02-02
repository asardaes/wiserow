#ifndef WISEROW_GENERICWORKERS_H_
#define WISEROW_GENERICWORKERS_H_

#include <cstddef> // size_t
#include <memory>
#include <string>
#include <utility>

#include <Rcpp.h>

#include "../core.h"
#include "../utils.h"
#include "../visitors.h"
#include "worker-strategies.h"

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
        , cumulative_(Rcpp::as<bool>(extras["cumulative"]))
        , ans_(ans)
        , arith_opr_(parse_arith_op(Rcpp::as<std::string>(extras["arith_op"])))
    { }

    virtual thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) override {
        bool need_init = arith_opr_.arith_op != ArithOp::ADD;

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
                supported_col_t variant = col_collection_(in_id, j);
                ans_(out_id, cumulative_ ? j : 0) = boost::apply_visitor(visitor_, variant);
                // this branch will never be reached from RowMeansWorker
            }
            else {
                supported_col_t variant = col_collection_(in_id, j);
                const T val = boost::apply_visitor(visitor_, variant);
                std::size_t prev_j = cumulative_ ? (j > 0 ? j - 1 : 0) : 0;
                ans_(out_id, cumulative_ ? j : 0) = arith_opr_.apply(ans_(out_id, prev_j), val);

                // for RowMeansWorker
                if (t_local) {
                    std::static_pointer_cast<CountStrategy>(t_local)->apply(0, variant, true);
                }
            }
        }

        return nullptr;
    }
protected:
    RowArithWorker(const OperationMetadata& metadata,
                   const ColumnCollection& cc,
                   OutputWrapper<T>& ans,
                   const Rcpp::List extras,
                   ArithmeticOperator&& arith_opr)
        : ParallelWorker(metadata, cc)
        , cumulative_(Rcpp::as<bool>(extras["cumulative"]))
        , ans_(ans)
        , arith_opr_(std::move(arith_opr))
    { }

    const bool cumulative_;
    OutputWrapper<T>& ans_;

    const T na_value_ = NA_REAL;
    const NAVisitor na_visitor_;

private:
    const ArithmeticOperator arith_opr_;
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
        , cumulative_(Rcpp::as<bool>(extras["cumulative"]))
        , ans_(ans)
        , arith_opr_(parse_arith_op(Rcpp::as<std::string>(extras["arith_op"])))
    { }

    virtual thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) override {
        bool need_init = arith_opr_.arith_op != ArithOp::ADD;

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
                supported_col_t variant = col_collection_(in_id, j);
                ans_(out_id, cumulative_ ? j : 0) = boost::apply_visitor(visitor_, variant);
                // this branch will never be reached from RowMeansWorker
            }
            else {
                supported_col_t variant = col_collection_(in_id, j);
                const int val = boost::apply_visitor(visitor_, variant);
                std::size_t prev_j = cumulative_ ? (j > 0 ? j - 1 : 0) : 0;
                ans_(out_id, cumulative_ ? j : 0) = arith_opr_.apply(ans_(out_id, prev_j), val);

                // for RowMeansWorker
                if (t_local) {
                    std::static_pointer_cast<CountStrategy>(t_local)->apply(0, variant, true);
                }
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

protected:
    RowArithWorker(const OperationMetadata& metadata,
                   const ColumnCollection& cc,
                   OutputWrapper<int>& ans,
                   const Rcpp::List extras,
                   ArithmeticOperator&& arith_opr)
        : ParallelWorker(metadata, cc)
        , cumulative_(Rcpp::as<bool>(extras["cumulative"]))
        , ans_(ans)
        , arith_opr_(std::move(arith_opr))
    { }

    const bool cumulative_;
    OutputWrapper<int>& ans_;

    const int na_value_ = NA_INTEGER;
    const NAVisitor na_visitor_;

private:
    const ArithmeticOperator arith_opr_;
    const NumericVisitor<int> visitor_;
};

// =================================================================================================

template<typename T>
class RowMeansWorker : public RowArithWorker<T>
{
public:
    RowMeansWorker(const OperationMetadata& metadata,
                   const ColumnCollection& cc,
                   OutputWrapper<T>& ans,
                   Rcpp::List extras)
        : RowArithWorker<T>(metadata, cc, ans, extras, ArithmeticOperator(ArithOp::ADD))
        , non_na_counter_(std::make_shared<CountStrategy>())
    { }

    virtual ParallelWorker::thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, ParallelWorker::thread_local_ptr t_local) override {
        std::shared_ptr<OutputStrategy<int>> thread_local_counter =
            t_local ? std::static_pointer_cast<OutputStrategy<int>>(t_local) : non_na_counter_->clone();

        thread_local_counter->reinit();
        RowArithWorker<T>::work_row(in_id, out_id, thread_local_counter);

        if (this->cumulative_) {
            double n = 0;
            for (std::size_t j = 0; j < this->col_collection_.ncol(); j++) {
                bool input_is_na = boost::apply_visitor(this->na_visitor_, this->col_collection_(in_id, j));

                if (input_is_na) {
                    if (this->metadata.na_action == NaAction::PASS) {
                        break;
                    }
                }
                else {
                    n += 1;
                }

                if (n > 0 && this->metadata.output_mode != LGLSXP) {
                    this->ans_(out_id, j) /= n;
                }
            }
        }
        else {
            T ans = this->ans_(out_id, 0);
            if (ans != this->na_value_) {
                double n = thread_local_counter->output(this->metadata, 0, false);

                if (this->metadata.output_mode != LGLSXP) {
                    this->ans_(out_id, 0) = ans / n;
                }
                else if (n == 0.0) {
                    // corner case: all values were NA
                    this->ans_(out_id, 0) = this->na_value_;
                }
            }
        }

        return thread_local_counter;
    }

private:
    const std::shared_ptr<CountStrategy> non_na_counter_;
};

} // namespace wiserow

#endif // WISEROW_GENERICWORKERS_H_
