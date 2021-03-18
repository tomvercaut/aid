#ifndef AID_INCLUDE_AID_MPSC_SENDER_HPP
#define AID_INCLUDE_AID_MPSC_SENDER_HPP

#include <aid/core/result.hpp>
#include <aid/mpsc/channel_type.hpp>

namespace aid::mpsc
{
template<typename T, ChannelType CT=ChannelType::OneShot>
{
public:
    auto send(T && t)->Result<std::void_t, SenderError<T>>
    {
        return ok(void);
    }
}
}// namespace aid::mpsc

#endif//AID_INCLUDE_AID_MPSC_SENDER_HPP
