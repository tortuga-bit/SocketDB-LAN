#include "methods.h"


void replace(std::string& text, const std::string& key, const std::string& value)
{
    size_t pos = 0;

    while ((pos = text.find(key, pos)) != std::string::npos)
    {
        text.replace(pos, key.length(), value);
        pos += value.length();
    }
}

void readMessage(int client_fd, ConnectionInfo& info) {
    char buffer[1024];
    std::string data;
    std::vector<char> response;
    HTTPRequest token;

    ssize_t bytes = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytes > 0) {
        buffer[bytes] = '\0';
        data = buffer;

        token = parse(data);
        response = dispatch(token, client_fd, info);
        send(client_fd, response.data(), response.size(), 0);
    }
}

std::string readFile(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

std::vector<char> get(HTTPRequest& request, int client_fd, const ConnectionInfo& info)
{
    std::string status = "200 OK";
    std::string contentType = "text/plain";
    std::vector<char> body;

    (void)client_fd;

    if (request.method != "GET")
    {
        status = "405 Method Not Allowed";
        contentType = "text/html";

        std::string error = readFile("public/404.html");
        body.assign(error.begin(), error.end());
    }
    else
    {
        std::string filePath;

        if (request.path == "/")
        {
            filePath = "public/index.html";
        }
        else
        {
            filePath = "public" + request.path;
        }

        body = readBinaryFile(filePath);

        if (body.empty())
        {
            status = "404 Not Found";
            contentType = "text/html";
            body = readBinaryFile("public/404.html");
        }
        else
        {
            contentType = getContentType(filePath);

            if (contentType == "text/html")
            {
                std::string html(body.begin(), body.end());
                replace(html, "{{SERVER_IP}}", info.serverIP);
                replace(html, "{{CLIENT_IP}}", info.clientIP);
                body.assign(html.begin(), html.end());
            }
        }
    }

    std::string header =
        "HTTP/1.1 " + status + "\r\n" +
        "Content-Type: " + contentType + "\r\n" +
        "Content-Length: " + std::to_string(body.size()) + "\r\n" +
        "Connection: close\r\n" +
        "\r\n";

    std::vector<char> response;
    response.insert(response.end(), header.begin(), header.end());
    response.insert(response.end(), body.begin(), body.end());

    return response;
}