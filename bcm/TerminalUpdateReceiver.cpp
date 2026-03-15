#include "TerminalUpdateReceiver.hpp"
#include <iostream>
#include "StateStructures.hpp"

void TerminalUpdateReceiver::start() {
    m_running = true;
    m_consoleThread = std::thread([this]{
        while(m_running) {
            std::cout << "Please enter new speed value: ";
            double speed;
            std::cin >> speed;
            SpeedState newSpeed;
            newSpeed.speed = speed;
            m_vehicleState->setSpeed(newSpeed);
        }
    });
}