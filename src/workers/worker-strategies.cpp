#include "worker-strategies.h"

#include <Rcpp.h> // NA_*

namespace wiserow {

// nocov start
void IdentityStrategy::apply(const std::size_t, const supported_col_t&, const bool) {
    throw "IdentityStrategy's apply() should not be called.";
}

int IdentityStrategy::output(const OperationMetadata&, const std::size_t, const bool) {
    throw "IdentityStrategy's output() should not be called.";
}
// nocov end

std::shared_ptr<OutputStrategy<int>> IdentityStrategy::clone() {
    return nullptr;
}

// =================================================================================================

BulkBoolStrategy::BulkBoolStrategy(const BulkBoolOp& bb_op, const NaAction& na_action)
    : bb_op_(bb_op)
    , logical_operator_(bb_op == BulkBoolOp::ALL ? BoolOp::AND : BoolOp::OR)
    , na_action_(na_action)
    , init_(bb_op == BulkBoolOp::ALL ? true : false)
    , flag_(init_)
{ }

void BulkBoolStrategy::reinit() {
    flag_ = init_;
}

bool BulkBoolStrategy::short_circuit() {
    // maybe don't break because R's all/any still check all values for NA when na.rm = FALSE
    if (na_action_ == NaAction::EXCLUDE) {
        if (bb_op_ == BulkBoolOp::ALL && !flag_) {
            return true;
        }
        else if (flag_ && (bb_op_ == BulkBoolOp::ANY || bb_op_ == BulkBoolOp::NONE)) {
            return true;
        }
    }

    return false;
}

void BulkBoolStrategy::apply(const std::size_t, const supported_col_t&, const bool match_flag) {
    flag_ = logical_operator_.apply(flag_, match_flag);
}

int BulkBoolStrategy::output(const OperationMetadata&, const std::size_t ncol, const bool any_na) {
    switch(bb_op_) {
    case BulkBoolOp::ALL: {
        if (ncol > 0) {
        if (flag_ && any_na) {
            return NA_INTEGER;
        }
        else {
            return flag_;
        }
    }
        else {
            return 0;
        }
    }
    case BulkBoolOp::ANY: {
        if (!flag_ && any_na) {
        return NA_INTEGER;
    }
        else {
            return flag_;
        }
    }
    case BulkBoolOp::NONE: {
        if (!flag_ && any_na) {
        return NA_INTEGER;
    }
        else {
            return !flag_;
        }
    }
    }

    return NA_INTEGER; // nocov
}

std::shared_ptr<OutputStrategy<int>> BulkBoolStrategy::clone() {
    return std::make_shared<BulkBoolStrategy>(this->bb_op_, this->na_action_);
}

// =================================================================================================

WhichFirstStrategy::WhichFirstStrategy()
    : which_(-1)
{ }

void WhichFirstStrategy::reinit() {
    which_ = -1;
}

bool WhichFirstStrategy::short_circuit() {
    return which_ >= 0;
}

void WhichFirstStrategy::apply(const std::size_t col, const supported_col_t&, const bool match_flag) {
    if (match_flag) {
        which_ = col;
    }
}

int WhichFirstStrategy::output(const OperationMetadata& metadata, const std::size_t, const bool) {
    if (which_ < 0) {
        return NA_INTEGER;
    }
    /*else if (metadata.cols.ptr) {
     return metadata.cols.ptr[which_]; TODO
    }*/
    else {
        return static_cast<int>(which_ + 1);
    }
}

std::shared_ptr<OutputStrategy<int>> WhichFirstStrategy::clone() {
    return std::make_shared<WhichFirstStrategy>();
}

// =================================================================================================

CountStrategy::CountStrategy()
    : count_(0)
{ }

void CountStrategy::reinit() {
    count_ = 0;
}

void CountStrategy::apply(const std::size_t, const supported_col_t&, const bool match_flag) {
    if (match_flag) {
        count_++;
    }
}

int CountStrategy::output(const OperationMetadata&, const std::size_t, const bool any_na) {
    if (any_na) {
        return NA_INTEGER;
    }
    else {
        return count_;
    }
}

std::shared_ptr<OutputStrategy<int>> CountStrategy::clone() {
    return std::make_shared<CountStrategy>();
}

} // namespace wiserow
