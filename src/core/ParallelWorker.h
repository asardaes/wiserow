#ifndef WISEROW_PARALLELWORKER_H_
#define WISEROW_PARALLELWORKER_H_

#define STRICT_R_HEADERS // collision between R.h and mingw_32/i686-w64-mingw32/include/windows.h

#include <cstddef> // std::size_t

#include <RcppParallel.h>
#include <RcppThread.h>

namespace wiserow {

// =================================================================================================

inline void parallel_for(std::size_t begin,
                         std::size_t end,
                         RcppParallel::Worker& worker,
                         std::size_t grain_size = 1)
{
    RcppParallel::parallelFor(begin, end, worker, grain_size);
    // always call after parallelFor to actually throw exception if there was an interruption
    RcppThread::checkUserInterrupt();
}

// =================================================================================================

class ParallelWorker : public RcppParallel::Worker
{
public:
    virtual ~ParallelWorker() = default;
    void operator()(std::size_t begin, std::size_t end) override final;

protected:
    ParallelWorker(const int interrupt_check_grain, const int min, const int max);
    virtual void work_it(std::size_t begin, std::size_t end) = 0;
    bool is_interrupted() const;
    bool is_interrupted(const std::size_t i) const;

    tthread::mutex mutex_;

private:
    int interrupt_grain(const int interrupt_check_grain, const int min, const int max) const;

    const int interrupt_grain_;
};

} // namespace wiserow

#endif // WISEROW_PARALLELWORKER_H_
