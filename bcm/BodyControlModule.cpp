#include <chrono>
#include <iostream>
#include "VehicleState.hpp"
#include <memory>
#include <thread>
#include "TerminalUpdateReceiver.hpp"

int main() {
    std::cout << "Hello QNX!" << std::endl;

    std::shared_ptr<VehicleState> vehicleState = std::make_shared<VehicleState>();

    TerminalUpdateReceiver updateReciver(vehicleState);
    updateReciver.start();

    while(true) {
        vehicleState->printState();
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    updateReciver.stop();

    return 0;
}