#ifndef WISEROW_OPERATIONMETADATA_H_
#define WISEROW_OPERATIONMETADATA_H_

#include <string>
#include <typeindex>
#include <vector>

#include <Rcpp.h>

namespace wiserow {

class OperationMetadata {
public:
    OperationMetadata(const Rcpp::List& metadata);

    const int num_workers;

    const std::string input_class;
    const std::vector<std::type_index> input_modes;

    const std::type_index output_mode;

private:
    static std::string get_string(const Rcpp::List& metadata, const std::string& key);
    static int get_int(const Rcpp::List& metadata, const std::string& key);
    static std::type_index parse_type(const std::string& type_str);
    static std::vector<std::type_index> parse_types(const Rcpp::StringVector& in_modes);
};

} // namespace wiserow

#endif // WISEROW_OPERATIONMETADATA_H_
