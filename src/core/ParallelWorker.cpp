#include "ParallelWorker.h"

namespace wiserow {

ParallelWorker::ParallelWorker(const OperationMetadata& metadata, const ColumnCollection& cc)
    : metadata(metadata)
    , col_collection_(cc)
    , interrupt_grain_(interrupt_grain(cc.nrow() / metadata.num_workers, 1000, 10000))
{ }

std::size_t ParallelWorker::num_ops() const {
    return col_collection_.nrow();
}

void ParallelWorker::operator()(std::size_t begin, std::size_t end) {
    if (eptr) return;

    try {
        work_it(begin, end);
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

bool ParallelWorker::is_interrupted() const {
    return RcppThread::isInterrupted();
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
