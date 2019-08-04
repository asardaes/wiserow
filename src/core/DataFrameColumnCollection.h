#ifndef WISEROW_DATAFRAMECOLUMNCOLLECTION_H_
#define WISEROW_DATAFRAMECOLUMNCOLLECTION_H_

#include <Rcpp.h>

#include "ColumnAbstractions.h"
#include "OperationMetadata.h"
#include "SurrogateColumn.h"

namespace wiserow {

class DataFrameColumnCollection : public ColumnCollection
{
public:
    DataFrameColumnCollection(const Rcpp::DataFrame& df, const OperationMetadata& metadata);
};

} // namespace wiserow

#endif // WISEROW_DATAFRAMECOLUMNCOLLECTION_H_
