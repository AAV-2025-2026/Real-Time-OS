#include "state_receiver.hpp"
#include <sys/socket.h>
#include <exception>
#include <cstdint>

namespace vehicle_state {

StateReceiver::StateReceiver(int portNumber = 5000) : m_portNumber(portNumber){}

void StateReceiver::start() {
    m_running = true;

    m_receiverThread = std::thread([&] {
        // Create socket
        int socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_descriptor < 0) {
            throw std::exception("Can't create socket");
        }
        struct sockaddr_in server_address, client_address;
        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(m_portNumber);

        // Bind socket
        int rc = bind(socket_descriptor, static_cast<struct sockaddr *>(&server_address), sizeof(server_address));
        if (rc < 0) {
            close(socket_descriptor);
            throw std::exception("Can't bind socket");
        }

        socklen_t address_len = sizeof(client_address);
        while(m_running) {
            // Receive data
            std::uint8_t buffer[m_bufferSize];
            ssize_t num_bytes = recvfrom(socket_descriptor, buffer, m_bufferSize - 1, 0, static_cast<struct sockaddr *>(&client_address), &address_len);
            if (num_bytes < 0) {
                std::cerr << "recvfrom failed";
                break;
            }
            buffer[num_bytes] = '\0';
            // Decode data
            std::uint32_t message_type = static_cast<std::uint32_t>(buffer)[0];

            switch (message_type) {
            case constant expression:
                /* code */
                break;
            default:
                break;
            }
            // Update vehicle state

        }
    });
}


} // namespace vehicle_state
