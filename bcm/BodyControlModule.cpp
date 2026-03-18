#include <chrono>
#include <cstdint>
#include <iostream>
#include "UDPClient.hpp"
#include "VehicleState.hpp"
#include <memory>
#include <thread>
#include "TerminalUpdateReceiver.hpp"

constexpr const char* PUBLISHER_IP = "192.168.1.100";
constexpr const uint16_t PUBLISHER_PORT_NUMBER = 9988;

int main() {
    std::cout << "Hello QNX!" << std::endl;

    auto publisher = std::make_shared<UDPClient>(PUBLISHER_IP, PUBLISHER_PORT_NUMBER);

    std::shared_ptr<VehicleState> vehicleState = std::make_shared<VehicleState>(publisher);

    TerminalUpdateReceiver updateReciver(vehicleState);
    updateReciver.start();

    while(true) {
        vehicleState->printState();
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    updateReciver.stop();

    return 0;
}