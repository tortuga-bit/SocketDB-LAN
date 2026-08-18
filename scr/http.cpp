#include "http.h"
#include "methods.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
// This function parses an HTTP request string and returns an HTTPRequest struct containing the method, path, and version.
HTTPRequest parse(const std::string& requestString)
{
    HTTPRequest request;
    std::string requestLine;
    std::istringstream stream(requestString);

    std::getline(stream, requestLine);

    if (requestLine.empty())
    {
        return request;
    }

    std::istringstream lineStream(requestLine);
    lineStream >> request.method >> request.path >> request.version;

    if (request.version.empty())
    {
        request.version = "HTTP/1.1";
    }

    return request;
}

std::vector<char> readBinaryFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        return {};
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return buffer;
}

std::string getContentType(const std::string& path)
{
    const std::string extension = path.substr(path.find_last_of('.') + 1);

    if (extension == "html" || extension == "htm")
    {
        return "text/html";
    }
    if (extension == "css")
    {
        return "text/css";
    }
    if (extension == "js")
    {
        return "application/javascript";
    }
    if (extension == "json")
    {
        return "application/json";
    }
    if (extension == "png")
    {
        return "image/png";
    }
    if (extension == "jpg" || extension == "jpeg")
    {
        return "image/jpeg";
    }
    if (extension == "svg")
    {
        return "image/svg+xml";
    }
    if (extension == "ico")
    {
        return "image/x-icon";
    }

    return "application/octet-stream";
}

std::vector<char> dispatch(HTTPRequest& request, int client_fd, const ConnectionInfo& info)
{
    if (request.method == "GET")
    {
        return get(request, client_fd, info);
    }
    else
    {
        std::string status = "405 Method Not Allowed";
        std::string contentType = "text/html";
        std::vector<char> body;

        std::string error = readFile("public/404.html");
        body.assign(error.begin(), error.end());

        return body;
    }
}

