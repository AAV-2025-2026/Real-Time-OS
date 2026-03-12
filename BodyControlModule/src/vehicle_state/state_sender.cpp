#include "state_sender.hpp"
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <mqueue.h>
#include "dbstruct.h"

namespace vehicle_state {

StateSender::StateSender(const char* server_ip, int port)
    : m_socketFD(-1) {
    initSocket(server_ip, port);
    initMQueue();
}

StateSender::~StateSender() {
    if (m_socketFD >= 0) {
        close(m_socketFD);
    }
}

void StateSender::initMQueue() {
    m_mqueue = mq_open("/db_queue", O_NONBLOCK);
}

void StateSender::initSocket(const char* server_ip, int port) {
    // Create UDP socket
    m_socketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socketFD < 0) {
        perror("Socket creation failed");
        return;
    }

    // Configure server address structure
    memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(port);
    
    // Convert IP address from string to binary
    if (inet_pton(AF_INET, server_ip, &m_serverAddr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(m_socketFD);
        m_socketFD = -1;
    }
}

void StateSender::sendLocation(const LocationData& location) {
    if (m_socketFD < 0) return;

    // Create a simple message format: "LOC,lat,lon"
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "LOC,%.8f,%.8f", 
             location.latitude, location.longitude);

    sendto(m_socketFD, buffer, strlen(buffer), 0,
           (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr));

    // Send to DB
    DB_t db_msg;
    strncpy(db_msg.table, "states", sizeof(db_msg.table));
    strncpy(db_msg.id, "GPS", sizeof(db_msg.id));
    strncpy(db_msg.msg, buffer, sizeof(buffer));
    int rc = mq_send(m_mqueue, (char *) &db_msg, sizeof(db_msg), 0);    
}

void StateSender::sendSpeed(const SpeedData& speed) {
    if (m_socketFD < 0) return;

    // Create a simple message format: "SPD,speed"
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "SPD,%.4f", speed.speed);

    sendto(m_socketFD, buffer, strlen(buffer), 0,
           (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr));

    // Send to DB
    DB_t db_msg;
    strncpy(db_msg.table, "states", sizeof(db_msg.table));
    strncpy(db_msg.id, "IMU", sizeof(db_msg.id));
    strncpy(db_msg.msg, buffer, sizeof(buffer));
    int rc = mq_send(m_mqueue, (char *) &db_msg, sizeof(db_msg), 0);   
}

void StateSender::sendGear(const Gear& gear) {
    if (m_socketFD < 0) return;

    // Create a simple message format: "GEAR,gearValue"
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "GEAR,%u", static_cast<unsigned int>(gear));

    sendto(m_socketFD, buffer, strlen(buffer), 0,
           (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr));
}

} // namespace vehicle_state

