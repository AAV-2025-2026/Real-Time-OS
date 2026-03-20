#ifndef STATE_STRUCTURES_HPP
#define STATE_STRUCTURES_HPP

#include <cstdint>
#include <chrono>
#include <ostream>

template <typename T>
class StateObject {
public:
    StateObject(const T& data) : m_data(data) {
        m_time = std::chrono::steady_clock::now();
    }
    T getData() const {
        return m_data;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> m_time;
    T m_data;
};

struct SpeedState {
    double speed;
};

struct DirectionState {
    double direction;
};

struct LocationState {
    double x, y;
};

struct IMUState {
    float ang_vel_x, ang_vel_y, ang_vel_z;
    float lin_acc_x, lin_acc_y, lin_acc_z;
};

enum class Gear : uint8_t {
    Park = 0,
    Drive = 1,
    Reverse = 2
};

inline std::ostream& operator<<(std::ostream& os, Gear g) {
    std::string name = "Invalid Gear";
    switch (g) {
        case Gear::Park: name = "Park"; break;
        case Gear::Drive: name = "Drive"; break;
        case Gear::Reverse: name = "Reverse"; break;
    }

    os << name;
    return os;
}

#endif
