#include "Metadata.h"

#include <typeinfo>
#include <utility> // std::move

namespace wiserow {

Metadata::Metadata(const Rcpp::List& metadata)
    : input_class(get_string(metadata, "input_class"))
    , output_mode(std::move(parse_type(get_string(metadata, "output_mode"))))
{
    Rcpp::StringVector in_modes = metadata["input_modes"];
    for (R_xlen_t i = 0; i < in_modes.length(); i++) {
        std::string in_mode = Rcpp::as<std::string>(in_modes(i));
        input_modes.push_back(std::move(parse_type(in_mode)));
    }
}

std::string Metadata::get_string(const Rcpp::List& metadata, const std::string& key) {
    return Rcpp::as<std::string>(metadata[key]);
}

std::type_index Metadata::parse_type(const std::string& type_str) {
    if (type_str == "integer") {
        return std::type_index(typeid(int));
    }
    else if (type_str == "double") {
        return std::type_index(typeid(double));
    }
    else {
        Rcpp::stop("[wiserow] unsupported type: " + type_str);
    }
}

} // namespace wiserow
