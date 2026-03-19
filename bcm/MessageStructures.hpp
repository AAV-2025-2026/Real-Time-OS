#ifndef MESSAGE_STRUCTURES_HPP
#define MESSAGE_STRUCTURES_HPP

#include <cstdint>

enum class MessageType : uint8_t {
    GPS = 0,
    IMU = 1,
    COMPASS = 2
};

#endif