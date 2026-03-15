#ifndef STATE_STRUCTURES_HPP
#define STATE_STRUCTURES_HPP

#include <chrono>

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

#endif