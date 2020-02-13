#include "integer-workers.h"

#include <stdexcept> // logic_error
#include <string>

#include <boost/variant/get.hpp>

namespace wiserow {

struct target_traits {
    bool is_na;
    char* char_target;
};

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

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

} // namespace wiserow
