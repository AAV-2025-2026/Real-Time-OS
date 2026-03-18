#ifndef VEHICLE_STATE_HPP
#define VEHICLE_STATE_HPP

#include <optional>
#include "StateStructures.hpp"
#include "UpdateSender.hpp"
#include <memory>

class VehicleState {
public:
    VehicleState(std::shared_ptr<UDPClient> updatePublisher);

    bool setSpeed(const SpeedState& newSpeed);
    std::optional<StateObject<SpeedState>> getSpeed() {return m_speed;}
    std::optional<StateObject<Gear>> getGear() {return m_gear;}

    bool setDirection(const DirectionState& newDirection);
    std::optional<StateObject<DirectionState>> getDirection() {return m_direction;}

    bool setLocation(const LocationState& newLocation);
    std::optional<StateObject<LocationState>> getLocation() {return m_location;}

    void printState();

private:
    std::shared_ptr<UpdateSender> m_updater;
    
    std::optional<StateObject<SpeedState>> m_speed;
    std::optional<StateObject<DirectionState>> m_direction;
    std::optional<StateObject<LocationState>> m_location;
    std::optional<StateObject<Gear>> m_gear;
};

#endif