#include "../wiserow.h"

#include <cstddef> // size_t
#include <memory>
#include <string>
#include <unordered_set>

#include <Rcpp.h>

#include "../core.h"
#include "../workers.h"

namespace wiserow {

std::shared_ptr<OutputWrapper<int>> get_wrapper_ptr(const OperationMetadata& metadata, SEXP output) {
    switch(metadata.output_class) {
    case RClass::VECTOR: {
        if (metadata.output_mode == LGLSXP) {
            Rcpp::LogicalVector ans(output);
            return std::make_shared<VectorOutputWrapper<LGLSXP, int>>(ans);
        }
        else {
            Rcpp::IntegerVector ans(output);
            return std::make_shared<VectorOutputWrapper<INTSXP, int>>(ans);
        }
    }
    case RClass::LIST: {
        Rcpp::List ans(output);

        if (metadata.output_mode == LGLSXP) {
            return std::make_shared<ListOutputWrapper<LGLSXP, int>>(ans);
        }
        else {
            return std::make_shared<ListOutputWrapper<INTSXP, int>>(ans);
        }
    }
    case RClass::DATAFRAME: {
        if (metadata.output_mode == LGLSXP) {
            return std::make_shared<DataFrameOutputWrapper<LGLSXP, int>>(output);
        }
        else {
            return std::make_shared<DataFrameOutputWrapper<INTSXP, int>>(output);
        }
    }
    case RClass::MATRIX: {
        if (metadata.output_mode == LGLSXP) {
            return std::make_shared<MatrixOutputWrapper<LGLSXP, int>>(output);
        }
        else {
            return std::make_shared<MatrixOutputWrapper<INTSXP, int>>(output);
        }
    }
    default: // nocov start
        Rcpp::stop("This operation does not support the chosen output class.");
    } // nocov end
}

// =================================================================================================

template<typename Worker>
void visit_with_match_type(SEXP metadata, SEXP data, SEXP output, const Rcpp::List extras) {
    std::string match_type = Rcpp::as<std::string>(extras["match_type"]);

    OperationMetadata metadata_(metadata);

    ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);
    std::size_t out_len = output_length(metadata_, col_collection);
    if (out_len == 0) return;

    std::shared_ptr<OutputWrapper<int>> wrapper_ptr = get_wrapper_ptr(metadata_, output);

    // always NaAction::Exclude to force short-circuit if appropriate
    if (match_type == "which_first") {
        auto out_strategy = std::make_shared<WhichFirstStrategy>();
        Worker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "count") {
        auto out_strategy = std::make_shared<CountStrategy>();
        Worker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "all") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ALL, NaAction::EXCLUDE);
        Worker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "any") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ANY, NaAction::EXCLUDE);
        Worker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "none") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::NONE, NaAction::EXCLUDE);
        Worker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else { // nocov start
        Rcpp::stop("Match type [" + match_type + "] not supported.");
    } // nocov end
}

// -------------------------------------------------------------------------------------------------

extern "C" SEXP row_finites(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    visit_with_match_type<FiniteTestWorker>(metadata, data, output, extras);
    return R_NilValue;
    END_RCPP
}

// -------------------------------------------------------------------------------------------------

extern "C" SEXP row_infs(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    visit_with_match_type<InfTestWorker>(metadata, data, output, extras);
    return R_NilValue;
    END_RCPP
}

// -------------------------------------------------------------------------------------------------

extern "C" SEXP row_nas(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    visit_with_match_type<NATestWorker>(metadata, data, output, extras);
    return R_NilValue;
    END_RCPP
}

// =================================================================================================

extern "C" SEXP row_compare(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    OperationMetadata metadata_(metadata);

    ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);
    std::size_t out_len = output_length(metadata_, col_collection);
    if (out_len == 0) return R_NilValue;

    Rcpp::List extras_(extras);
    std::string match_type = Rcpp::as<std::string>(extras_["match_type"]);
    SEXP comp_op = extras_["comp_op"];
    SEXP target_val = extras_["target_val"];

    std::shared_ptr<OutputWrapper<int>> wrapper_ptr = get_wrapper_ptr(metadata_, output);

    if (match_type == "all") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ALL, metadata_.na_action);
        CompBasedWorker worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "any") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ANY, metadata_.na_action);
        CompBasedWorker worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "none") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::NONE, metadata_.na_action);
        CompBasedWorker worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "which_first") {
        auto out_strategy = std::make_shared<WhichFirstStrategy>();
        CompBasedWorker worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "count") {
        auto out_strategy = std::make_shared<CountStrategy>();
        CompBasedWorker worker(metadata_, col_collection, *wrapper_ptr, comp_op, target_val, out_strategy);
        parallel_for(worker);
    }
    else { // nocov start
        Rcpp::stop("Match type [" + match_type + "] not supported.");
    } // nocov end

    return R_NilValue;
    END_RCPP
}

// =================================================================================================

extern "C" SEXP row_in(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    OperationMetadata metadata_(metadata);

    ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);
    std::size_t out_len = output_length(metadata_, col_collection);
    if (out_len == 0) return R_NilValue;

    Rcpp::List extras_(extras);
    std::string match_type = Rcpp::as<std::string>(extras_["match_type"]);
    SEXP target_sets = extras_["target_sets"];
    bool negate = Rcpp::as<bool>(extras_["negate"]);

    std::shared_ptr<OutputWrapper<int>> wrapper_ptr = get_wrapper_ptr(metadata_, output);

    if (match_type == "all") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ALL, metadata_.na_action);
        InSetWorker worker(metadata_, col_collection, *wrapper_ptr, target_sets, negate, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "any") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ANY, metadata_.na_action);
        InSetWorker worker(metadata_, col_collection, *wrapper_ptr, target_sets, negate, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "none") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::NONE, metadata_.na_action);
        InSetWorker worker(metadata_, col_collection, *wrapper_ptr, target_sets, negate, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "which_first") {
        auto out_strategy = std::make_shared<WhichFirstStrategy>();
        InSetWorker worker(metadata_, col_collection, *wrapper_ptr, target_sets, negate, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "count") {
        auto out_strategy = std::make_shared<CountStrategy>();
        InSetWorker worker(metadata_, col_collection, *wrapper_ptr, target_sets, negate, out_strategy);
        parallel_for(worker);
    }
    else { // nocov start
        Rcpp::stop("Match type [" + match_type + "] not supported.");
    } // nocov end

    return R_NilValue;
    END_RCPP
}

// =================================================================================================

extern "C" SEXP row_duplicated(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    OperationMetadata metadata_(metadata);

    ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);
    std::size_t out_len = output_length(metadata_, col_collection);
    if (out_len == 0) return R_NilValue;

    Rcpp::List extras_(extras);
    std::string match_type = Rcpp::as<std::string>(extras_["match_type"]);

    std::shared_ptr<OutputWrapper<int>> wrapper_ptr = get_wrapper_ptr(metadata_, output);

    if (match_type == "NULL") {
        auto out_strategy = std::make_shared<IdentityStrategy>();
        DuplicatedWorker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "any") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::ANY, metadata_.na_action);
        DuplicatedWorker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "none") {
        auto out_strategy = std::make_shared<BulkBoolStrategy>(BulkBoolOp::NONE, metadata_.na_action);
        DuplicatedWorker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "which_first") {
        auto out_strategy = std::make_shared<WhichFirstStrategy>();
        DuplicatedWorker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else if (match_type == "count") {
        auto out_strategy = std::make_shared<CountStrategy>();
        DuplicatedWorker worker(metadata_, col_collection, *wrapper_ptr, out_strategy);
        parallel_for(worker);
    }
    else { // nocov start
        Rcpp::stop("Match type [" + match_type + "] not supported.");
    } // nocov end

    return R_NilValue;
    END_RCPP
}

// =================================================================================================

template<int RT, typename T>
void numeric_row_extrema(const OperationMetadata& metadata,
                         const ColumnCollection& col_collection,
                         const Rcpp::List& extras,
                         SEXP output) {
    std::shared_ptr<OutputWrapper<T>> output_wrapper = nullptr;

    switch(metadata.output_class) {
    case RClass::VECTOR: {
        Rcpp::Vector<RT> ans(output);
        output_wrapper = std::make_shared<VectorOutputWrapper<RT, T>>(ans);
        break;
    }
    case RClass::LIST: {
        output_wrapper = std::make_shared<ListOutputWrapper<RT, T>>(output);
        break;
    }
    case RClass::DATAFRAME: {
        output_wrapper = std::make_shared<DataFrameOutputWrapper<RT, T>>(output);
        break;
    }
    case RClass::MATRIX: {
        Rcpp::Matrix<RT> ans(output);
        output_wrapper = std::make_shared<MatrixOutputWrapper<RT, T>>(ans);
        break;
    }
    default: // nocov start
        Rcpp::stop("This operation does not support the chosen output class.");
    } // nocov end

    RowExtremaWorker<T> worker(metadata, col_collection, *output_wrapper, extras);
    parallel_for(worker);
}

extern "C" SEXP row_extrema(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    OperationMetadata metadata_(metadata);

    ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);
    std::size_t out_len = output_length(metadata_, col_collection);
    if (out_len == 0) return R_NilValue;

    Rcpp::List extras_(extras);

    if (metadata_.output_mode == LGLSXP) {
        numeric_row_extrema<LGLSXP, int>(metadata_, col_collection, extras_, output);
    }
    else if (metadata_.output_mode == INTSXP) {
        numeric_row_extrema<INTSXP, int>(metadata_, col_collection, extras_, output);
    }
    else if (metadata_.output_mode == REALSXP) {
        numeric_row_extrema<REALSXP, double>(metadata_, col_collection, extras_, output);
    }
    else {
        std::unordered_set<std::string> temp_strings;
        RowExtremaWorker<boost::string_ref> worker(metadata_, col_collection, extras_, temp_strings);
        parallel_for(worker);

        switch(metadata_.output_class) {
        case RClass::VECTOR: {
            Rcpp::StringVector ans(output);
            for (R_xlen_t i = 0; i < ans.length(); i++) {
                ans[i] = worker.ans[i].data();
            }
            break;
        }
        case RClass::LIST: {
            Rcpp::List list(output);
            for (R_xlen_t i = 0; i < list.length(); i++) {
                Rcpp::StringVector ans(list[i]);
                ans[0] = worker.ans[i].data();
            }
            break;
        }
        case RClass::DATAFRAME: {
            Rcpp::DataFrame df(output);
            Rcpp::StringVector ans(df[0]);
            for (R_xlen_t i = 0; i < ans.length(); i++) {
                ans[i] = worker.ans[i].data();
            }
            break;
        }
        case RClass::MATRIX: {
            Rcpp::StringMatrix ans(output);
            for (int i = 0; i < ans.nrow(); i++) {
                ans[i] = worker.ans[i].data();
            }
            break;
        }
        default: // nocov start
            Rcpp::stop("This operation does not support the chosen output class.");
        } // nocov end
    }

    return R_NilValue;
    END_RCPP
}

} // namespace wiserow
