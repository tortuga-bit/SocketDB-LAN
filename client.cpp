#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#define PORT 8080

int main() {
  int sock = 0;
  struct sockaddr_in6 serv_addr;
  const char *datos = nullptr;
  std::string respuesta, mensaje;

  // Create socket
  sock = socket(AF_INET6, SOCK_STREAM, 0);
  if (sock < 0) {
    std::cerr << "Socket creation error" << std::endl;
    return -1;
  }

  memset(&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin6_family = AF_INET6;
  serv_addr.sin6_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary
  if (inet_pton(AF_INET6, "2806:106e:1a:8902:529a:479d:1531:7910", &serv_addr.sin6_addr) <= 0) {
    std::cerr << "Invalid address/ Address not supported" << std::endl;
    return -1;
  }


  // Connect to server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cerr << "Connection Failed" << std::endl;
    perror("connect");
    return -1;
  }


  std::cout << "Datos para servidor con formato:" << std::endl;
  std::cout << "Suma: Add num1 + num2" << std::endl;
  std::cout << "Repeticion: Echo mensaje" << std::endl;
  // Leer mensaje del usuario
  std::getline(std::cin, mensaje);
  // Convertir mensaje a formato C-string para enviar al servidor
  datos = mensaje.c_str();


  // Send data
  send(sock, datos, strlen(datos), 0);
  char buffer[1024] = {0};
  memset(buffer, 0, sizeof(buffer));

  // Recibir respuesta del servidor
  int bytes_recibidos = recv(sock, buffer, sizeof(buffer), 0);
  respuesta = std::string(buffer);
  std::cout << "El servidor respondio: " << respuesta << std::endl;


  // Close socket
  close(sock);
  return 0;
}