#ifndef WISEROW_OPERATIONMETADATA_H_
#define WISEROW_OPERATIONMETADATA_H_

#include <string>
#include <vector>

#include <Rcpp.h>

namespace wiserow {

typedef int r_t;

class OperationMetadata {
public:
    OperationMetadata(const Rcpp::List& metadata);

    const int num_workers;

    const std::string input_class;
    const std::vector<r_t> input_modes;

    const r_t output_mode;

private:
    static std::string get_string(const Rcpp::List& metadata, const std::string& key);
    static int get_int(const Rcpp::List& metadata, const std::string& key);
    static r_t parse_type(const std::string& type_str);
    static std::vector<r_t> parse_types(const Rcpp::StringVector& in_modes);
};

} // namespace wiserow

#endif // WISEROW_OPERATIONMETADATA_H_
