#ifndef AID_INCLUDE_AID_MPSC_MPSC_QUEUE_HPP
#define AID_INCLUDE_AID_MPSC_MPSC_QUEUE_HPP

#include <aid/core/result.hpp>
#include <aid/mpsc/mpsc_error.hpp>
#include <mutex>
#include <queue>
namespace aid::mpsc
{
template<typename T>
class MpscQueue
{
public:
    void push(T &&value)
    {
        std::scoped_lock lock(Mut);
        Q.push(std::move(value));
    }
    auto pop() -> Result<T, MpscError>
    {
        std::scoped_lock lock(Mut);
        if (!Q.empty()) {
            // move an existing value out of the queue while having the lock
            auto owner = std::move(Q.front());
            Q.pop();
            return aid::core::ok<T, MpscError>(std::move(owner));
        }
        return aid::core::err<T, MpscError>(MpscError::EmptyQueue);
    }

private:
    std::queue<T> Q;
    std::mutex Mut;
};
}// namespace aid::mpsc

#endif//AID_INCLUDE_AID_MPSC_MPSC_QUEUE_HPP
