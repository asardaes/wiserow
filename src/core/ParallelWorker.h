#ifndef WISEROW_PARALLELWORKER_H_
#define WISEROW_PARALLELWORKER_H_

#define STRICT_R_HEADERS // collision between R.h and mingw_32/i686-w64-mingw32/include/windows.h

#include <cstddef> // std::size_t
#include <exception>
#include <memory>

#include <RcppParallel.h>
#include <RcppThread.h>

#include "ColumnAbstractions.h"
#include "OperationMetadata.h"

namespace wiserow {

class WorkerThreadLocal
{
public:
    virtual ~WorkerThreadLocal() = default;
};

// =================================================================================================

class ParallelWorker : public RcppParallel::Worker
{
public:
    virtual ~ParallelWorker() = default;

    void operator()(std::size_t begin, std::size_t end) override final;

    std::size_t num_ops() const;

    const OperationMetadata metadata;
    std::exception_ptr eptr;

protected:
    typedef std::shared_ptr<WorkerThreadLocal> thread_local_ptr;

    ParallelWorker(const OperationMetadata& metadata, const ColumnCollection& cc);

    virtual thread_local_ptr work_row(std::size_t in_id, std::size_t out_id, thread_local_ptr t_local) = 0;

    const ColumnCollection col_collection_;
    tthread::mutex mutex_;

private:
    int interrupt_grain(const int interrupt_check_grain, const int min, const int max) const;

    std::size_t corresponding_row(std::size_t id) const;

    bool is_interrupted(const std::size_t i) const;

    const int interrupt_grain_;
};

// =================================================================================================

inline void parallel_for(ParallelWorker& worker) {
    std::size_t num_ops = worker.num_ops();
    if (num_ops < 1) {
        return;
    }

    double grain = num_ops / worker.metadata.num_workers / 10;
    if (grain < 1000) {
        grain = 1000;
    }

    RcppParallel::parallelFor(0, num_ops, worker, static_cast<std::size_t>(grain));

    if (worker.eptr) {
        std::rethrow_exception(worker.eptr);
    }

    // always call after parallelFor to actually throw exception if there was an interruption
    RcppThread::checkUserInterrupt();
}

} // namespace wiserow

#endif // WISEROW_PARALLELWORKER_H_
