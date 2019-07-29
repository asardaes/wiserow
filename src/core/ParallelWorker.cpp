#include "ParallelWorker.h"

namespace wiserow {

void ParallelWorker::operator()(std::size_t begin, std::size_t end) {
    work_it(begin, end);
    // make sure this is called at least once per thread call
    RcppThread::isInterrupted();
}

ParallelWorker::ParallelWorker(const int interrupt_check_grain, const int min, const int max)
    : interrupt_grain_(interrupt_grain(interrupt_check_grain, min, max))
{ }

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
