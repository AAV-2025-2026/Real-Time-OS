#ifndef MESSAGE_STRUCTURES_HPP
#define MESSAGE_STRUCTURES_HPP

#include <cstdint>

enum class MessageType : uint8_t {
    GPS = 0,
    IMU = 1
};

enum class PublishType : uint8_t {
    LOCATION = 0,
    SPEED = 1,
    GEAR = 2
};

#endif