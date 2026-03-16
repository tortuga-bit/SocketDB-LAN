#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <vector>
#include <string>


#define PORT 8080

int add(int a, int b);
std::vector<std::string> parse(const std::string& input);
std::string dispatch(const std::vector<std::string>& datos);

// --- Función para crear el socket del servidor ---
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
    // data y token son variables de tipo string por que el cliente manda informacion en string
    std::string data, message;
    // token es un vector de strings del cliente
    std::vector<std::string> token;
    
    // read() lee datos del socket cliente y los almacena en el buffer. 
    // read() retorna el número de bytes leídos, o -1 si ocurre un error.
    ssize_t bytes = read(client_fd, buffer, sizeof(buffer) - 1);

    //Limpia el buffer para evitar datos residuales
    if (bytes > 0) {
        
        buffer[bytes] = '\0';  // importante terminar la cadena
        // Convierte el buffer de binario a string para facilitar el procesamiento
        data = buffer;
        // data es una sola cadena que contiene el mensaje completo enviado por el cliente
        // parse() divide esa cadena en tokens individuales en un vector
        token = parse(data);
        // mandamos el vector de tokens al dispatch para que determine la respuesta adecuada según el comando recibido
        // ademas dispatch ejecuta la función correspondiente al comando
        message = dispatch(token);
        
        //mandamos la respuesta a la peticion del cliente
        send(client_fd, message.c_str(), message.size(), 0);

        
    }

    
}

// --- Función para dividir un string en tokens por espacio ---
std::vector<std::string> parse(const std::string& input) {
    std::vector<std::string> tokens;
    // token es una variable temporal que se utiliza para construir 
    // cada token a medida que se recorre el string de entrada.
    std::string token;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == ' ') {
            if (!token.empty()) {
                // Si el token no está vacío, lo agregamos a la lista de tokens y limpiamos el token para el siguiente
                tokens.push_back(token);
                token.clear();
            }
        } else if (input[i] == '\n' || input[i] == '\r') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += input[i];
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}


//
std::string dispatch(const std::vector<std::string>& datos)
{
    if(datos[0] == "Add")
    {
        try {
            int n1, n2;
            // Intentamos convertir los strings a enteros
            n1 = std::stoi(datos[1]);
            n2 = std::stoi(datos[2]);

            // Si llegamos aquí, es que AMBOS son enteros válidos
            int resultado = add(n1, n2);
            return std::to_string(resultado);

        } catch (const std::invalid_argument& e) {
            // Esto se ejecuta si datos[1] o datos[2] NO eran números (ej: "abc")
            return "Error: Uno de los argumentos no es un numero.";

        } catch (const std::out_of_range& e) {
            // Esto se ejecuta si el número es demasiado grande para un 'int'
            return "Error: Numero demasiado grande.";
        }
    }

    if(datos[0] == "Echo")
        return datos[1];

    return "ERROR";
}

// --- Funciones de ejemplo para el dispatch ---
int add(int a, int b)
{
    return a + b;
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


