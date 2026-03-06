#ifndef STATE_RECEIVER_HPP
#define STATE_RECEIVER_HPP

#include "vehicle_state.hpp"
#include <thread>

namespace vehicle_state {

class StateReceiver {
public:
    StateReceiver(int portNumber = 5000);
    ~StateReceiver();

private:
    VehicleState m_vehicleState;
    std::thread m_receiverThread;
    int m_portNumber;
    bool m_running;
};

} // namespace vehicle_state

#endif
