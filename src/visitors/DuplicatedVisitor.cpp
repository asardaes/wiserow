#include "boolean-visitors.h"

#include <algorithm> // find

namespace wiserow {

// TODO: maybe call promote_to before/after na_visitor depending on na_action

bool DuplicatedVisitor::operator()(const bool val) {
    switch(current_type_) {
    case Type::BOOL:
        return !bools_.insert(val).second;
    case Type::INT:
        return !ints_.insert(static_cast<int>(val)).second;
    case Type::DOUBLE:
        return !doubles_.insert(static_cast<double>(val)).second;
    case Type::COMPLEX: {
        std::complex<double> cplx(static_cast<double>(val), 0);
        bool ans = std::find(complexs_.begin(), complexs_.end(), cplx) != complexs_.end();
        if (!ans) complexs_.push_back(cplx);
        return ans;
    }
    case Type::STRING:
        return !strings_.insert(::wiserow::to_string(val)).second;
    }

    throw "Unreachable code reached..."; // nocov
}

bool DuplicatedVisitor::operator()(const int val) {
    if (na_visitor_(val)) return handle_na();

    promote_to(Type::INT);

    switch(current_type_) {
    case Type::BOOL:
    case Type::INT:
        return !ints_.insert(val).second;
    case Type::DOUBLE:
        return !doubles_.insert(static_cast<double>(val)).second;
    case Type::COMPLEX: {
        std::complex<double> cplx(val, 0);
        bool ans = std::find(complexs_.begin(), complexs_.end(), cplx) != complexs_.end();
        if (!ans) complexs_.push_back(cplx);
        return ans;
    }
    case Type::STRING:
        return !strings_.insert(::wiserow::to_string(val)).second;
    }

    throw "Unreachable code reached..."; // nocov
}

bool DuplicatedVisitor::operator()(const double val) {
    if (na_visitor_(val)) return handle_na();

    promote_to(Type::DOUBLE);

    switch(current_type_) {
    case Type::BOOL:
    case Type::INT:
    case Type::DOUBLE:
        return !doubles_.insert(static_cast<double>(val)).second;
    case Type::COMPLEX: {
        std::complex<double> cplx(val, 0);
        bool ans = std::find(complexs_.begin(), complexs_.end(), cplx) != complexs_.end();
        if (!ans) complexs_.push_back(cplx);
        return ans;
    }
    case Type::STRING:
        return !strings_.insert(::wiserow::to_string(val)).second;
    }

    throw "Unreachable code reached..."; // nocov
}

bool DuplicatedVisitor::operator()(const std::complex<double>& val) {
    if (na_visitor_(val)) return handle_na();

    promote_to(Type::COMPLEX);

    switch(current_type_) {
    case Type::BOOL:
    case Type::INT:
    case Type::DOUBLE:
    case Type::COMPLEX: {
        bool ans = std::find(complexs_.begin(), complexs_.end(), val) != complexs_.end();
        if (!ans) complexs_.push_back(val);
        return ans;
    }
    case Type::STRING:
        return !strings_.insert(::wiserow::to_string(val)).second;
    }

    throw "Unreachable code reached..."; // nocov
}

bool DuplicatedVisitor::operator()(const boost::string_ref val) {
    if (na_visitor_(val)) return handle_na();

    promote_to(Type::STRING);

    switch(current_type_) {
    case Type::BOOL:
    case Type::INT:
    case Type::DOUBLE:
    case Type::COMPLEX:
    case Type::STRING:
        return !strings_.insert(val.to_string()).second;
    }

    throw "Unreachable code reached..."; // nocov
}

void DuplicatedVisitor::promote_to(Type type) {
    switch(type) {
    case Type::BOOL:
    case Type::INT:
        if (current_type_ >= Type::INT) break;
        ints_.insert(bools_.begin(), bools_.end());
        current_type_ = Type::INT;
        break;
    case Type::DOUBLE:
        if (current_type_ >= Type::DOUBLE) break;

        if (current_type_ == Type::BOOL) {
            doubles_.insert(bools_.begin(), bools_.end());
        }
        else if (current_type_ == Type::INT) {
            doubles_.insert(ints_.begin(), ints_.end());
        }

        current_type_ = Type::DOUBLE;
        break;
    case Type::COMPLEX:
        if (current_type_ >= Type::COMPLEX) break;

        if (current_type_ == Type::BOOL) {
            for (bool val : bools_) {
                complexs_.push_back(std::complex<double>(val, 0));
            }
        }
        else if (current_type_ == Type::INT) {
            for (int val : ints_) {
                complexs_.push_back(std::complex<double>(val, 0));
            }
        }
        else if (current_type_ == Type::DOUBLE) {
            for (double val : doubles_) {
                complexs_.push_back(std::complex<double>(val, 0));
            }
        }

        current_type_ = Type::COMPLEX;
        break;
    case Type::STRING:
        if (current_type_ >= Type::STRING) break;

        if (current_type_ == Type::BOOL) {
            for (bool val : bools_) {
                strings_.insert(::wiserow::to_string(val));
            }
        }
        else if (current_type_ == Type::INT) {
            for (int val : ints_) {
                strings_.insert(::wiserow::to_string(val));
            }
        }
        else if (current_type_ == Type::DOUBLE) {
            for (double val : doubles_) {
                strings_.insert(::wiserow::to_string(val));
            }
        }
        else if (current_type_ == Type::COMPLEX) {
            for (std::complex<double>& val : complexs_) {
                strings_.insert(::wiserow::to_string(val));
            }
        }

        current_type_ = Type::STRING;
        break;
    }
}

bool DuplicatedVisitor::handle_na() {
    if (seen_na_) {
        return true;
    }
    else {
        seen_na_ = true;
        return false;
    }
}

} // namespace wiserow
