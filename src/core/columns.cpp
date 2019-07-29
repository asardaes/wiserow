#include "columns.h"

#include <typeinfo>
#include <typeindex>
#include <utility> // std::move

namespace wiserow {

// =================================================================================================

ColumnCollectionMetadata::ColumnCollectionMetadata(const Rcpp::List& metadata)
    : num_workers(get_int(metadata, "num_workers"))
    , input_class(get_string(metadata, "input_class"))
    , output_mode(std::move(parse_type(get_string(metadata, "output_mode"))))
{
    Rcpp::StringVector in_modes = metadata["input_modes"];
    for (R_xlen_t i = 0; i < in_modes.length(); i++) {
        std::string in_mode = Rcpp::as<std::string>(in_modes(i));
        input_modes.push_back(std::move(parse_type(in_mode)));
    }
}

std::string ColumnCollectionMetadata::get_string(const Rcpp::List& metadata, const std::string& key) {
    return Rcpp::as<std::string>(metadata[key]);
}

int ColumnCollectionMetadata::get_int(const Rcpp::List& metadata, const std::string& key) {
    return Rcpp::as<int>(metadata[key]);
}

std::type_index ColumnCollectionMetadata::parse_type(const std::string& type_str) {
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

// =================================================================================================

ColumnCollection ColumnCollection::coerce(ColumnCollectionMetadata metadata, SEXP data) {
    if (metadata.input_class == "matrix") {
        if (metadata.input_modes[0] == typeid(int)) {
            return MatrixColumnCollection<Rcpp::IntegerMatrix>(data);
        }
        else {
            return MatrixColumnCollection<Rcpp::NumericMatrix>(data);
        }
    }
    else {
        Rcpp::stop("[wiserow] unsupported input class: " + metadata.input_class);
    }
}

ColumnCollection::ColumnCollection(const std::size_t nrow)
    : nrow_(nrow)
{ }

std::size_t ColumnCollection::ncol() const {
    return columns_.size();
}

std::size_t ColumnCollection::nrow() const {
    return nrow_;
}

const VariantColumn& ColumnCollection::operator[](const std::size_t j) const {
    return *(columns_[j]);
}

const supported_col_t ColumnCollection::operator()(const std::size_t i, const std::size_t j) const {
    return (*(columns_[j]))[i];
}

} // namespace wiserow
