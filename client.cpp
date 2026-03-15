#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#define PORT 8080

int main() {
  int sock = 0;
  struct sockaddr_in serv_addr;
  const char *datos = nullptr;
  std::string respuesta, mensaje;

  // Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    std::cerr << "Socket creation error" << std::endl;
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    std::cerr << "Invalid address/ Address not supported" << std::endl;
    return -1;
  }


  // Connect to server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cerr << "Connection Failed" << std::endl;
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