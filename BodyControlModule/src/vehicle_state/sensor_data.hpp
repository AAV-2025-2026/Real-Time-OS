#ifndef SENSOR_DATA_HPP
#define SENSOR_DATA_HPP

#include <cstdint>
namespace vehicle_state {

/**
 * Definitions of structs that hold various types of sensor data
 */

struct SpeedData {
    double speed;
};

struct DirectionData {
    double heading; // Subject to change // Degrees from north
};

struct LocationData {
    double latitude, longitude;
};

enum class Gear : uint8_t {
    Park = 0,
    Drive = 1,
    Reverse = 2
};

} // namespace vehicle_state

#endif
