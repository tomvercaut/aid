#ifndef AID_INCLUDE_AID_MPSC_RECEIVER_HPP
#define AID_INCLUDE_AID_MPSC_RECEIVER_HPP

#include <aid/core/result.hpp>
#include <aid/mpsc/channel_type.hpp>

namespace aid::mpsc
{
template<typename T, ChannelType CT>
class Receiver
{
    template<typename T, ChannelType CT = ChannelType::OneShot>
    auto recv(T &&t) -> Result<std::void_t, RecvError<T>>
    {
        return ::aid::core::ok<void>;
    }
};
}// namespace aid::mpsc

#endif//AID_INCLUDE_AID_MPSC_RECEIVER_HPP
