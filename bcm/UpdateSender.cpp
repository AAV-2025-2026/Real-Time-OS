#include "UpdateSender.hpp"
#include "StateStructures.hpp"
#include <iostream>

UpdateSender::UpdateSender() {
    // Implementation coming in the future
}

bool UpdateSender::updateSpeed(const SpeedState& newSpeed) {
    std::cout << "I have sent the updated state to the relevant parties" << std::endl;
    return true;
}