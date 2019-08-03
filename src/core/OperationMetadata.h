#ifndef WISEROW_OPERATIONMETADATA_H_
#define WISEROW_OPERATIONMETADATA_H_

#include <cstddef> // size_t
#include <string>
#include <vector>

#include <Rcpp.h>

namespace wiserow {

typedef int R_vec_t;

struct surrogate_vector {
    surrogate_vector(const int * const ptr, const std::size_t len, const bool is_null)
        : ptr(ptr), len(len), is_null(is_null)
    { }

    const int * const ptr;
    const std::size_t len;
    const bool is_null;
};

class OperationMetadata {
public:
    OperationMetadata(const Rcpp::List& metadata);

    const int num_workers;

    const std::string input_class;
    const std::vector<R_vec_t> input_modes;

    const R_vec_t output_mode;

    const std::string na_action;

    const surrogate_vector cols;
};

} // namespace wiserow

#endif // WISEROW_OPERATIONMETADATA_H_
