#ifndef STATE_SENDER_HPP
#define STATE_SENDER_HPP

#include "sensor_data.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
namespace vehicle_state {
class StateSender {
public:
    StateSender(const char* server_ip = "127.0.0.1", int port = 5005);
    ~StateSender();
    void sendLocation(const LocationData&);
    void sendSpeed(const SpeedData&);
    void sendGear(const Gear&);
private:
    int socket_fd;
    struct sockaddr_in server_addr;
    void initSocket(const char* server_ip, int port);
};
}

#endif