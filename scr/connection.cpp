#include "connection.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

int createServerSocket(int port) {

    int server_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    //defines una estructura de tipo sockaddr_in,
    //que sirve para almacenar información sobre la dirección de red (IP y puerto).
    struct sockaddr_in6 address;
    
    // indica el tipo de dirreccion ip que se va a usar, en este caso AF_INET6 para IPv6
    address.sin6_family = AF_INET6;

    //sin_addr.s_addr indica la IP del servidor a la que se va a “ligar” el socket.
    //INADDR_ANY es una ipv4 que equivale a 0.0.0.0, lo que significa que el servidor escuchará en todas las interfaces de red disponibles en la máquina.
    // aqui indicamos por que ip el servidor va a escuchar, en este caso por todas las ip disponibles en la máquina.
    address.sin6_addr = in6addr_any;

    //sin_port es el número de puerto donde el servidor escuchará.
    address.sin6_port = htons(port);
    
    //bind() asocia el socket con la dirección y puerto especificados
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //listen() pone el socket en modo de escucha, permitiendo que el servidor acepte conexiones entrantes.
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << port << std::endl;
    return server_fd;
}

int acceptClient(int server_fd, ConnectionInfo& info)
{

    sockaddr_in6 client_addr;
    socklen_t addrlen = sizeof(client_addr);

    int client_fd = accept(
        server_fd,
        (struct sockaddr *)&client_addr,
        &addrlen
    );

    if (client_fd < 0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }


    // =========================
    // IP DEL CLIENTE
    // =========================

    char client_ip[INET6_ADDRSTRLEN];

    inet_ntop(
        AF_INET6,
        &client_addr.sin6_addr,
        client_ip,
        sizeof(client_ip)
    );

    info.clientIP = client_ip;

    std::cout << "Client connected from: "
              << info.clientIP
              << ":"
              << ntohs(client_addr.sin6_port)
              << std::endl;


    // =========================
    // IP DEL SERVIDOR
    // =========================

    sockaddr_in6 server_addr;
    socklen_t server_len = sizeof(server_addr);

    if (getsockname(
            client_fd,
            (struct sockaddr *)&server_addr,
            &server_len
        ) < 0)
    {
        perror("getsockname failed");
    }
    else
    {
        char server_ip[INET6_ADDRSTRLEN];

        inet_ntop(
            AF_INET6,
            &server_addr.sin6_addr,
            server_ip,
            sizeof(server_ip)
        );

        info.serverIP = server_ip;

        std::cout << "Server IP: "
                  << info.serverIP
                  << std::endl;
    }

    return client_fd;
}