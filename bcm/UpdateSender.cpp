#include "UpdateSender.hpp"
#include "StateStructures.hpp"
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <mqueue.h>
#include <string>
#include <thread>
#include "../Database/dbstruct.h"
#include <string.h>
#include <vector>

bool sendDBMsg(const mqd_t& mq, std::string_view tableName, const int id, const std::vector<char> &data, const unsigned int priority) {    
    DB_t msg;

    if (data.size() > sizeof(msg.table) || tableName.size() > sizeof(msg.table)) {
        std::cerr << "Attempting to send too much data in mqueue" << std::endl;
        return false;
    }

    strncpy(msg.table, tableName.data(), sizeof(msg.table));
    strncpy(msg.id, std::to_string(id).c_str(), sizeof(msg.id));
    strncpy(msg.msg, data.data(), sizeof(msg.msg));

    int rc = mq_send(mq, reinterpret_cast<char *>(&msg), sizeof(msg), priority);
    if (rc == -1) {
        char * error = strerror(errno);
        std::cerr << "Failed to send data: \n" << std::string(data.data()) << "\n to table: " << tableName << " with ID: " << id << "\n";
        std::cerr << "Error code: " << error << std::endl;
        return false;
    }

    std::cout << "Successfully sent data: \n" << std::string(data.data()) << "\n to table: " << tableName << " with ID: " << id << std::endl;
    return true;
}

UpdateSender::UpdateSender() {
    std::cout << "Trying to open mqueue" << std::endl;
    while (true) {
        m_mqueue = mq_open(DB_MQUEUE_NAME, O_WRONLY);
        if (m_mqueue != static_cast<mqd_t>(-1)) {
            break;
        }
        std::cerr << "Could not open mqueue. ";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cerr << "Trying to open mqueue again" << std::endl;
    }
    std::cout << "mqueue successfully opened\n";


}

bool UpdateSender::updateSpeed(const SpeedState& newSpeed) {
    if (!updateSpeedDB(newSpeed)) {
        std::cerr << "Failed to update the database with a new speed value" << std::endl;
        return false;
    }
    if (!updateSpeedROS(newSpeed)) {
        std::cerr << "Failed to update ROS with a new speed value" << std::endl;
        return false;
    }
    
    std::cout << "I have sent the updated state to the relevant parties" << std::endl;
    return true;
}

bool UpdateSender::updateSpeedDB(const SpeedState& newSpeed) {
    std::string speedString = "Speed: " + std::to_string(newSpeed.speed);
    std::vector<char> data(speedString.begin(), speedString.end());
    data.push_back('\0');

    return sendDBMsg(m_mqueue, "sensors", 1, data, MQ_PRIORITY);
}

bool UpdateSender::updateSpeedROS(const SpeedState& newSpeed) {
    std::cout << "Unimplemented speed update over ROS\n";
    return true;
}