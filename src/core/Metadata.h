#ifndef WISEROW_METADATA_H_
#define WISEROW_METADATA_H_

#include <string>
#include <typeindex>
#include <vector>

#include <Rcpp.h>

namespace wiserow {

class Metadata {
public:
    Metadata(const Rcpp::List& metadata);

    const std::string input_class;
    const std::type_index output_mode;

    std::vector<std::type_index> input_modes;

private:
    static std::string get_string(const Rcpp::List& metadata, const std::string& key);
    static std::type_index parse_type(const std::string& type_str);
};

} // namespace wiserow

#endif // WISEROW_METADATA_H_
