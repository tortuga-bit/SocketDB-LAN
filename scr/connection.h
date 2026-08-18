#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>

struct ConnectionInfo
{
    std::string serverIP;
    std::string clientIP;
};

int createServerSocket(int port);

int acceptClient(int server_fd, ConnectionInfo& info);

#endif