#ifndef AID_INCLUDE_AID_MPSC_MPSC_ERROR_HPP
#define AID_INCLUDE_AID_MPSC_MPSC_ERROR_HPP

namespace aid::mpsc
{
template<typename T>
class SenderError
{
public:
    explicit SenderError(T &&value) : Value{std::move(value)} {}

private:
    T Value;
};

enum MpscError {
    Sender,
    Receiver,
    EmptyQueue
};

}// namespace aid::mpsc

#endif//AID_INCLUDE_AID_MPSC_MPSC_ERROR_HPP
