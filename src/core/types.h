#ifndef WISEROW_TYPES_H_
#define WISEROW_TYPES_H_

#include "boost/variant/variant.hpp"

namespace wiserow {

typedef boost::variant<int, double> supported_col_t;

} // namespace wiserow

#endif // WISEROW_TYPES_H_
