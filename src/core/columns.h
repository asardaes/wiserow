#ifndef WISEROW_COLUMNS_H_
#define WISEROW_COLUMNS_H_

#include <complex>
#include <cstddef> // size_t
#include <memory> // shared_ptr
#include <vector>

#include <boost/utility/string_ref.hpp>
#include <boost/variant/variant.hpp>

#include "OperationMetadata.h"

namespace wiserow {

typedef boost::variant<int, double, boost::string_ref, std::complex<double>> supported_col_t;

// =================================================================================================

class VariantColumn
{
public:
    virtual ~VariantColumn() {}
    virtual const supported_col_t operator[](const std::size_t id) const = 0;
};

// =================================================================================================

class ColumnCollection
{
public:
    static ColumnCollection coerce(const OperationMetadata& metadata, SEXP data);

    ColumnCollection(const std::size_t nrow);
    virtual ~ColumnCollection() {}

    std::size_t nrow() const;
    std::size_t ncol() const;

    const supported_col_t operator()(const std::size_t i, const std::size_t j) const;

protected:
    std::vector<std::shared_ptr<const VariantColumn>> columns_;
    const std::size_t nrow_;
};

} // namespace wiserow

#endif // WISEROW_COLUMNS_H_
