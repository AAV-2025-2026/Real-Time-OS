#ifndef UPDATE_SENDER_HPP
#define UPDATE_SENDER_HPP

#include "StateStructures.hpp"

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

};

#endif