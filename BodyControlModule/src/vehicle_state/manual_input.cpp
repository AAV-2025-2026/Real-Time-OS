#include "manual_input.hpp"
#include "sensor_data.hpp"
#include <memory>
#include <iostream>
#include <thread>

namespace vehicle_state {

ManualInput::ManualInput(std::shared_ptr<VehicleState> vehicleState) : m_vehicleState{vehicleState} {}

void ManualInput::readInput() {
    m_running = true;
    m_readThread = std::thread([this]{
        while(m_running) {
            std::cout << "Which Vehicle State would you like to update?\n"
            << "1. Position, 2. Speed" << std::endl;
            int choice;
            std::cin >> choice;
            if (choice == 1) {
                std::cout << "Position Selected\nEnter x coordinate:";
                double x_coord, y_coord;
                std::cin >> x_coord;
                std::cout << "Enter y coordinate:";
                std::cin >> y_coord;

                LocationData new_position = {x_coord, y_coord};

                m_vehicleState->setLocation(new_position);
            } else if (choice == 2) {
                std::cout << "Speed Selected\nEnter speed:";
                double speed;
                std::cin >> speed;

                SpeedData new_speed = {speed};
                m_vehicleState->setSpeed(new_speed);
            } else {
                std::cout << "Invalid choice, please try again\n";
            }
        }
    });
}

void ManualInput::stop() {
    m_running = false;
}

}
