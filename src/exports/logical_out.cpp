#include "../wiserow.h"

#include <cstddef> // size_t
#include <memory>
#include <string>

#include <Rcpp.h>

#include "../workers/workers.h"
#include "helpers.h"

namespace wiserow {

extern "C" SEXP row_nas(SEXP metadata, SEXP data, SEXP output, SEXP extras) {
    BEGIN_RCPP
    OperationMetadata metadata_(metadata);

    ColumnCollection col_collection = ColumnCollection::coerce(metadata_, data);
    std::size_t out_len = output_length(metadata_, col_collection);
    if (out_len == 0) return R_NilValue;

    Rcpp::List extras_(extras);
    std::string bulk_comp_op = Rcpp::as<std::string>(extras_["bulk_comp_op"]);

    std::shared_ptr<OutputWrapper<int>> wrapper_ptr;
    switch(metadata_.output_class) {
    case OutputClass::vector: {
        Rcpp::LogicalVector ans(output);
        wrapper_ptr = std::make_shared<VectorOutputWrapper<LGLSXP, int>>(ans);
        break;
    }
    case OutputClass::list: {
        Rcpp::List ans(output);
        wrapper_ptr = std::make_shared<ListOutputWrapper<LGLSXP, int>>(ans);
        break;
    }
    }

    if (bulk_comp_op == "all") {
        NATestWorker worker(metadata_, col_collection, *wrapper_ptr, BulkCompOp::ALL);
        parallel_for(worker);
    }
    else if (bulk_comp_op == "any") {
        NATestWorker worker(metadata_, col_collection, *wrapper_ptr, BulkCompOp::ANY);
        parallel_for(worker);
    }
    else if (bulk_comp_op == "none") {
        NATestWorker worker(metadata_, col_collection, *wrapper_ptr, BulkCompOp::NONE);
        parallel_for(worker);
    }

    return R_NilValue;
    END_RCPP
}

} // namespace wiserow
