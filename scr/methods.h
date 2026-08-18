#ifndef METHODS_H
#define METHODS_H

#include "connection.h"
#include "http.h"

#include <string>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

#include <vector>

void replace(std::string& text, const std::string& key, const std::string& value);
void readMessage(int client_fd, ConnectionInfo& info);
std::string readFile(const std::string& path);
std::vector<char> get(HTTPRequest& request, int client_fd, const ConnectionInfo& info);

#endif
