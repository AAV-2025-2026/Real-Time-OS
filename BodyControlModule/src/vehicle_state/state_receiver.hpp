#ifndef STATE_RECEIVER_HPP
#define STATE_RECEIVER_HPP

#include "vehicle_state.hpp"
#include <thread>

namespace vehicle_state {

class StateReceiver {
public:
    StateReceiver(int portNumber = 5000);
    ~StateReceiver();

    void start();

private:
    VehicleState m_vehicleState;
    std::thread m_receiverThread;
    static constexpr size_t m_bufferSize{2048};
    int m_portNumber;
    bool m_running;
};

} // namespace vehicle_state

#endif
