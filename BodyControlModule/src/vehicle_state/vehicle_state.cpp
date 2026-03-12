#include "vehicle_state.hpp"
#include "sensor_data.hpp"

namespace vehicle_state {

VehicleState::VehicleState() : m_sender("127.0.0.1", 5000) {
}

void VehicleState::setSpeed(const SpeedData& value) {
    m_speed = std::make_pair(std::chrono::steady_clock::now(), value);
    m_sender.sendSpeed(value);

    if (value.speed > 0) {
        m_sender.sendGear(Gear::Drive);
    } else if (value.speed < 0) {
        m_sender.sendGear(Gear::Reverse);
    } else {
        m_sender.sendGear(Gear::Park);
    }
}

void VehicleState::setDirection(const DirectionData& value) {

}

void VehicleState::setLocation(const LocationData& value){
    m_location = std::make_pair(std::chrono::steady_clock::now(), value);
    m_sender.sendLocation(value);
}

} // namespace vehicle_state
