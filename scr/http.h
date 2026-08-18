#ifndef HTTP_H
#define HTTP_H

#include "connection.h"

#include <string>
#include <vector>

#include <fstream>
#include <sstream>


struct HTTPRequest
{
    std::string method;
    std::string path;
    std::string version;
};

HTTPRequest parse(const std::string& requestString);
std::vector<char> dispatch(HTTPRequest& request, int client_fd, const ConnectionInfo& info);
std::vector<char> readBinaryFile(const std::string& path);
std::string getContentType(const std::string& path);

#endif