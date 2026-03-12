#include "state_sender.hpp"
#include <cstdio>
#include <unistd.h>

namespace vehicle_state {

StateSender::StateSender(const char* server_ip, int port)
    : socket_fd(-1) {
    initSocket(server_ip, port);
}

StateSender::~StateSender() {
    if (socket_fd >= 0) {
        close(socket_fd);
    }
}

void StateSender::initSocket(const char* server_ip, int port) {
    // Create UDP socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Socket creation failed");
        return;
    }

    // Configure server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Convert IP address from string to binary
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(socket_fd);
        socket_fd = -1;
    }
}

void StateSender::sendLocation(const LocationData& location) {
    if (socket_fd < 0) return;

    // Create a simple message format: "LOC,lat,lon"
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "LOC,%.8f,%.8f", 
             location.latitude, location.longitude);

    sendto(socket_fd, buffer, strlen(buffer), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void StateSender::sendSpeed(const SpeedData& speed) {
    if (socket_fd < 0) return;

    // Create a simple message format: "SPD,speed"
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "SPD,%.4f", speed.speed);

    sendto(socket_fd, buffer, strlen(buffer), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void StateSender::sendGear(const Gear& gear) {
    if (socket_fd < 0) return;

    // Create a simple message format: "GEAR,gearValue"
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "GEAR,%u", static_cast<unsigned int>(gear));

    sendto(socket_fd, buffer, strlen(buffer), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));
}

} // namespace vehicle_state

