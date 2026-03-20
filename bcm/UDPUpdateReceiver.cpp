#include "UDPUpdateReceiver.hpp"
#include "MessageStructures.hpp"
#include "UDPClient.hpp"
#include <cstring>
#include <memory>
#include <iostream>
#include "StateStructures.hpp"

UDPUpdateReceiver::UDPUpdateReceiver(std::shared_ptr<VehicleState> vehicleState, std::shared_ptr<UDPClient> subscriber) :
    UpdateReceiver(vehicleState),
    m_subscriber(subscriber) {

}

void UDPUpdateReceiver::start() {
    m_running = true;
    m_receiverThread = std::thread([this] {
        while(m_running) {
            auto inputPacket = m_subscriber->receiveData(MAX_RECEIVE_LENGTH);
            if (inputPacket.size() > 0) {
                MessageType messageType = static_cast<MessageType>(inputPacket[0]);
                switch (messageType) {
                    case MessageType::GPS:
                        std::cout << "Received GPS packet\n";
                        LocationState receivedLocation;
                        std::memcpy(&receivedLocation, static_cast<void *>(&inputPacket[1]), sizeof(receivedLocation));
                        m_vehicleState->setLocation(receivedLocation);
                        break;
                    case MessageType::IMU:
                        std::cout << "Received IMU packet\n";
                        IMUState receivedIMU;
                        std::memcpy(&receivedIMU, static_cast<void *>(&inputPacket[1]), sizeof(receivedIMU));
                        m_vehicleState->setIMU(receivedIMU);
                        break;
                    case MessageType::COMPASS:
                        std::cout << "Received Compass packet\n";
                        DirectionState receivedDirection;
                        std::memcpy(&receivedDirection, static_cast<void *>(&inputPacket[1]), sizeof(receivedDirection));
                        m_vehicleState->setDirection(receivedDirection);
                        break;
                    default:
                        std::cerr << "Received invalid packet" << std::endl;
                        break;
                }
            }
        }
    });
}

void UDPUpdateReceiver::stop() {
    m_running = false;
}
