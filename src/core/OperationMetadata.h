#ifndef WISEROW_OPERATIONMETADATA_H_
#define WISEROW_OPERATIONMETADATA_H_

#include <cstddef> // size_t
#include <vector>

#include <Rcpp.h>

namespace wiserow {

typedef int R_vec_t;

enum class NaAction {
    EXCLUDE,
    PASS
};

enum class RClass {
    VECTOR,
    LIST,
    DATAFRAME,
    MATRIX
};

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

    const RClass input_class;
    const std::vector<R_vec_t> input_modes;

    const RClass output_class;
    const R_vec_t output_mode;

    const NaAction na_action;

    const surrogate_vector cols;
    const surrogate_vector rows;

    const R_vec_t factor_mode;
};

} // namespace wiserow

#endif // WISEROW_OPERATIONMETADATA_H_
