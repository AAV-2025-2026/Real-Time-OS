#include "state_receiver.hpp"
#include <sys/socket.h>
#include <exception>
#include <cstdint>

namespace vehicle_state {

StateReceiver::StateReceiver(int portNumber = 5000) : m_portNumber(portNumber), m_running(true) {
    m_receiverThread = std::thread([&] {
        // Create socket
        int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);
        if (socket_descriptor == -1) {
            throw std::exception("Can't create socket");
        }

        while(m_running) {
            // Receive data
            constexpr size_t buffer_size = 2048;
            std::uint8_t buffer[buffer_size];
            ssize_t num_bytes = recvfrom(socket_descriptor, buffer, buffer_size, );

            // Decode data

            // Update vehicle state

        }
    });
}


} // namespace vehicle_state
