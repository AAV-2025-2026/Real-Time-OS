#ifndef UPDATE_SENDER_HPP
#define UPDATE_SENDER_HPP

#include "StateStructures.hpp"
#include <vector>
#include <mqueue.h>
#include <string_view>

bool sendDBMsg(const mqd_t& mq, std::string_view tableName, const int id, const std::vector<char>& data, const unsigned int priority);

/**
    This Class Has to update relevant components when the vehicle state is updated
    1. Database through mqueue
    2. ROS Subscribers
        a. Through UDP
        b. Through ROS 2
    
*/
class UpdateSender {
public:
    UpdateSender();
    bool updateSpeed(const SpeedState& newSpeed);

private:
    bool updateSpeedDB(const SpeedState& newSpeed);
    bool updateSpeedROS(const SpeedState& newSpeed);

    static constexpr const char* DB_MQUEUE_NAME = "/db_queue";
    static constexpr const int MQ_PRIORITY = 1;
    mqd_t m_mqueue;

};

#endif