#ifndef AID_INCLUDE_AID_MPSC_CHANNEL_HPP
#define AID_INCLUDE_AID_MPSC_CHANNEL_HPP

#include <aid/mpsc/channel_type.hpp>
#include <utility>

namespace aid::mpsc {

template<typename T, ChannelType CT>
class Sender;
template<typename T, ChannelType CT>
class Receiver;

    template<typename T, ChannelType CT>
    std::pair<Sender<T, CT>,Receiver<T,CT> channel() {
        auto queue = std::make_shared<MpscQueue>();
        return std::pair{Sender{queue}, Receiver{queue}};
    }
}

#endif//AID_INCLUDE_AID_MPSC_CHANNEL_HPP
