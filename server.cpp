#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <cstring>

#define PORT 8080

// --- Función para crear el socket del servidor ---
int createServerSocket(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    //defines una estructura de tipo sockaddr_in,
    //que sirve para almacenar información sobre la dirección de red (IP y puerto).
    struct sockaddr_in address;
    
    // indica el tipo de dirreccion ip que se va a usar, en este caso AF_INET para IPv4
    address.sin_family = AF_INET;

    //sin_addr.s_addr indica la IP del servidor a la que se va a “ligar” el socket.
    //INADDR_ANY es una ipv4 que equivale a 0.0.0.0, lo que significa que el servidor escuchará en todas las interfaces de red disponibles en la máquina.
    // aqui indicamos por que ip el servidor va a escuchar, en este caso por todas las ip disponibles en la máquina.
    address.sin_addr.s_addr = INADDR_ANY;

    //sin_port es el número de puerto donde el servidor escuchará.
    address.sin_port = htons(port);
    
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

// --- Función para aceptar clientes y mostrar su IP ---
int acceptClient(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_fd < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Client connected from: " << client_ip 
              << ":" << ntohs(client_addr.sin_port) << std::endl;

    return client_fd;
}

// --- Función para leer un mensaje del cliente ---
void readMessage(int client_fd) {
    char buffer[1024];

    // read() lee datos del socket cliente y los almacena en el buffer.
    ssize_t bytes = read(client_fd, buffer, sizeof(buffer) - 1);

    //Limpia el buffer para evitar datos residuales
    if (bytes > 0) {
        buffer[bytes] = '\0';  // importante terminar la cadena
        std::cout << "Message from client: " << buffer << std::endl;
    }
}

// --- Función principal ---
int main() {
    int server_fd = createServerSocket(PORT);
    
    while (true) {
        int client_fd = acceptClient(server_fd);
        readMessage(client_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}


