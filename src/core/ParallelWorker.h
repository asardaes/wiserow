#ifndef WISEROW_PARALLELWORKER_H_
#define WISEROW_PARALLELWORKER_H_

#define STRICT_R_HEADERS // collision between R.h and mingw_32/i686-w64-mingw32/include/windows.h

#include <cstddef> // std::size_t
#include <exception>

#include <RcppParallel.h>
#include <RcppThread.h>

#include "OperationMetadata.h"

namespace wiserow {

// =================================================================================================

class ParallelWorker : public RcppParallel::Worker
{
public:
    virtual ~ParallelWorker() = default;
    void operator()(std::size_t begin, std::size_t end) override final;

    std::exception_ptr eptr;

protected:
    ParallelWorker(const OperationMetadata& metadata, const int interrupt_check_grain, const int min, const int max);

    virtual void work_it(std::size_t begin, std::size_t end) = 0;

    bool is_interrupted() const;
    bool is_interrupted(const std::size_t i) const;

    const OperationMetadata metadata_;
    tthread::mutex mutex_;

private:
    int interrupt_grain(const int interrupt_check_grain, const int min, const int max) const;

    const int interrupt_grain_;
};

// =================================================================================================

inline void parallel_for(std::size_t begin,
                         std::size_t end,
                         ParallelWorker& worker,
                         std::size_t grain_size = 1)
{
    RcppParallel::parallelFor(begin, end, worker, grain_size);
    if (worker.eptr) {
        std::rethrow_exception(worker.eptr);
    }

    // always call after parallelFor to actually throw exception if there was an interruption
    RcppThread::checkUserInterrupt();
}

} // namespace wiserow

#endif // WISEROW_PARALLELWORKER_H_
