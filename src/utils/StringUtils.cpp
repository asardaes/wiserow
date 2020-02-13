#include "StringUtils.h"

#include <stdexcept>

#include "../visitors.h"

namespace wiserow {

static std::shared_ptr<BooleanVisitor> inf_visitor = BooleanVisitorBuilder().is_inf().build();

// =================================================================================================

// dummy for some templates, shouldn't be used after optimizations
std::string to_string(const boost::string_ref val) { // nocov start
    throw std::runtime_error("Unwanted conversion between boost::string_ref and std::string.");
} // nocov end

std::string to_string(const bool val) {
    return val ? "TRUE" : "FALSE";
}

std::string to_string(const int val) {
    return std::to_string(val);
}

std::string to_string(const double val) {
    if ((*inf_visitor)(val)) {
        return val > 0 ? "Inf" : "-Inf";
    }
    else {
        // https://stackoverflow.com/a/13709929/5793905
        std::string str = std::to_string(val);
        str.erase(str.find_last_not_of("0") + 1, std::string::npos);
        str.erase(str.find_last_not_of(".") + 1, std::string::npos);
        return str;
    }
}

std::string to_string(const std::complex<double>& val) {
    double imag = val.imag();

    if ((*inf_visitor)(val)) {
        std::string real = val.real() > 0 ? "Inf" : "-Inf";

        if (imag >= 0) {
            return real + "+" + ::wiserow::to_string(imag) + "i";
        }
        else {
            return real + ::wiserow::to_string(imag) + "i";
        }
    }
    else {
        if (imag >= 0) {
            return ::wiserow::to_string(val.real()) + "+" + ::wiserow::to_string(imag) + "i";
        }
        else {
            return ::wiserow::to_string(val.real()) + ::wiserow::to_string(imag) + "i";
        }
    }
}

} // namespace wiserow
