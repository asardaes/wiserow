#ifndef WISEROW_STRINGUTILS_H_
#define WISEROW_STRINGUTILS_H_

#include <complex>
#include <string>

#include <boost/utility/string_ref.hpp>

namespace wiserow {

std::string to_string(const boost::string_ref val);
std::string to_string(const bool val);
std::string to_string(const int val);
std::string to_string(const double val);
std::string to_string(const std::complex<double>& val);

} // namespace wiserow

#endif // WISEROW_STRINGUTILS_H_
