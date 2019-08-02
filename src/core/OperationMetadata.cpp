#include "OperationMetadata.h"

#include <utility> // move

namespace wiserow {

// =================================================================================================

OperationMetadata::OperationMetadata(const Rcpp::List& metadata)
    : num_workers(get_int(metadata, "num_workers"))
    , input_class(get_string(metadata, "input_class"))
    , input_modes(parse_types(metadata["input_modes"]))
    , output_mode(std::move(parse_type(get_string(metadata, "output_mode"))))
    , na_action(get_string(metadata, "na_action"))
{ }

std::string OperationMetadata::get_string(const Rcpp::List& metadata, const std::string& key) {
    return Rcpp::as<std::string>(metadata[key]);
}

int OperationMetadata::get_int(const Rcpp::List& metadata, const std::string& key) {
    return Rcpp::as<int>(metadata[key]);
}

r_t OperationMetadata::parse_type(const std::string& type_str) {
    if (type_str == "integer") {
        return INTSXP;
    }
    else if (type_str == "double") {
        return REALSXP;
    }
    else if (type_str == "logical") {
        return LGLSXP;
    }
    else if (type_str == "character") {
        return STRSXP;
    }
    else {
        Rcpp::stop("[wiserow] unsupported type: " + type_str);
    }
}

std::vector<r_t> OperationMetadata::parse_types(const Rcpp::StringVector& in_modes) {
    std::vector<r_t> input_modes;

    for (R_xlen_t i = 0; i < in_modes.length(); i++) {
        std::string in_mode = Rcpp::as<std::string>(in_modes(i));
        input_modes.push_back(std::move(parse_type(in_mode)));
    }

    return input_modes;
}

} // namespace wiserow
