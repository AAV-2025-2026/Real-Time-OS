#include <iostream>
#include "VehicleState.hpp"
#include <memory>
#include "TerminalUpdateReceiver.hpp"

int main() {
    std::cout << "Hello QNX!" << std::endl;

    std::shared_ptr<VehicleState> vehicleState = std::make_shared<VehicleState>();

    TerminalUpdateReceiver updateReciver(vehicleState);
    updateReciver.start();

    while(true);

    updateReciver.stop();
    
    return 0;
}