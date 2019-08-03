#include "ParallelWorker.h"

namespace wiserow {

ParallelWorker::ParallelWorker(const OperationMetadata& metadata, const ColumnCollection& cc)
    : metadata(metadata)
    , col_collection_(cc)
    , interrupt_grain_(interrupt_grain(this->num_ops() / metadata.num_workers, 1000, 10000))
{ }

std::size_t ParallelWorker::num_ops() const {
    if (metadata.rows.ptr) {
        return metadata.rows.len;
    }
    else if (metadata.rows.is_null) {
        return col_collection_.nrow();
    }
    else {
        return 0;
    }
}

void ParallelWorker::operator()(std::size_t begin, std::size_t end) {
    if (eptr) return;

    try {
        for (std::size_t id = begin; id < end; id++) {
            if (eptr || is_interrupted(id)) break;

            work_row(corresponding_row(id));
        }
    }
    catch(...) {
        mutex_.lock();
        if (!eptr) eptr = std::current_exception();
        mutex_.unlock();
        return;
    }

    // make sure this is called at least once per thread call
    RcppThread::isInterrupted();
}

std::size_t ParallelWorker::corresponding_row(std::size_t id) const {
    return metadata.rows.ptr ? metadata.rows.ptr[id] - 1 : id;
}

bool ParallelWorker::is_interrupted(const std::size_t i) const {
    return RcppThread::isInterrupted(i % interrupt_grain_ == 0);
}

// how often to check for user interrupt inside a thread
int ParallelWorker::interrupt_grain(const int interrupt_check_grain, const int min, const int max) const {
    int result = interrupt_check_grain / 10;
    if (result < min) return min;
    if (result > max) return max;
    if (result < 1) return 1;
    return result;
}

} // namespace wiserow
