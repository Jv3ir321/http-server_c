#include "server.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  // Se definen estructuras, y variables necesarias

  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;

  // Socket de escucha
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  socklen_t client_len = sizeof(address);

  if (server_fd < 0) {
    perror("Socket error");
    exit(-1);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("Setsockpt failed!");
    exit(-1);
  }
  // AF_INET hace referencia a IPv4, que es como nos referimos a ips de 32 bits
  // ej: 192.198.1.68
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  // htons() traduce Host To Network Short y convierte un entero corto de 16
  // bits desde el host al orden de bytes que se usa en la red (big-endian)
  address.sin_port = htons(PORT);
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind Failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("Socket successfully bound to port %d\n", PORT);

  if (listen(server_fd, 3) < 0) {
    perror("Error during listen!");
    exit(-1);
  }

  printf("Listening!");
  // Aceptar cliente
  new_socket = accept(server_fd, (struct sockaddr *)&address, &client_len);

  if (new_socket < 0) {
    perror("Error while accepting client");
    exit(-1);
  }

  printf("Client Accepted!");
  // Recibir Datos

  send_fd(new_socket);
  read_fd(new_socket);

  close(new_socket);
  close(server_fd);

  return 0;
}
