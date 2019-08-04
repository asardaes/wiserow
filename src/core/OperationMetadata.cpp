#include "OperationMetadata.h"

#include <string>

namespace wiserow {

int get_int(const Rcpp::List& metadata, const std::string& key) {
    return Rcpp::as<int>(metadata[key]);
}

std::string get_string(const Rcpp::List& metadata, const std::string& key) {
    return Rcpp::as<std::string>(metadata[key]);
}

InputClass parse_input_class(const Rcpp::List& metadata) {
    std::string str = get_string(metadata, "input_class");

    if (str == "matrix") {
        return InputClass::MATRIX;
    }
    else if (str == "data.frame") {
        return InputClass::DATAFRAME;
    }
    else {
        Rcpp::stop("[wiserow] unsupported input class: " + str);
    }
}

OutputClass parse_output_class(const Rcpp::List& metadata) {
    std::string str = get_string(metadata, "output_class");

    if (str == "vector") {
        return OutputClass::VECTOR;
    }
    else if (str == "list") {
        return OutputClass::LIST;
    }
    else {
        Rcpp::stop("[wiserow] unsupported output class: " + str);
    }
}

R_vec_t parse_mode(const std::string& mode_str) {
    if (mode_str == "integer") {
        return INTSXP;
    }
    else if (mode_str == "double") {
        return REALSXP;
    }
    else if (mode_str == "logical") {
        return LGLSXP;
    }
    else if (mode_str == "character") {
        return STRSXP;
    }
    else if (mode_str == "complex") {
        return CPLXSXP;
    }
    else {
        Rcpp::stop("[wiserow] unsupported mode: " + mode_str);
    }
}

std::vector<R_vec_t> parse_modes(const Rcpp::StringVector& in_modes) {
    std::vector<R_vec_t> input_modes;

    for (R_xlen_t i = 0; i < in_modes.length(); i++) {
        std::string in_mode = Rcpp::as<std::string>(in_modes(i));
        input_modes.push_back(parse_mode(in_mode));
    }

    return input_modes;
}

NaAction parse_na_action(const Rcpp::List& metadata) {
    std::string str = get_string(metadata, "na_action");

    if (str == "pass") {
        return NaAction::PASS;
    }
    else {
        return NaAction::EXCLUDE;
    }
}

surrogate_vector coerce_subset_indices(SEXP ids) {
    if (Rf_isNull(ids)) {
        return surrogate_vector(nullptr, 0, true);
    }
    else {
        Rcpp::IntegerVector vec(ids);

        if (vec.length() > 0) {
            return surrogate_vector(&vec[0], vec.length(), false);
        }
        else {
            return surrogate_vector(nullptr, 0, false);
        }
    }
}

// =================================================================================================

OperationMetadata::OperationMetadata(const Rcpp::List& metadata)
    : num_workers(get_int(metadata, "num_workers"))
    , input_class(parse_input_class(metadata))
    , input_modes(parse_modes(metadata["input_modes"]))
    , output_class(parse_output_class(metadata))
    , output_mode(parse_mode(get_string(metadata, "output_mode")))
    , na_action(parse_na_action(metadata))
    , cols(coerce_subset_indices(metadata["cols"]))
    , rows(coerce_subset_indices(metadata["rows"]))
{ }

} // namespace wiserow
