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

// el proposito de parsear request http, es traducir cadenas de texto sin
// significado que vienen a traves de un network socket en variables que la
// aplicadion puede interpretar y actuar con esta

// dividimoos el body de la request con esa funcion, y se usa el caracter \0
// para indicar que se termine la string ahi, y los separe adecuadamente,
// luego como strstr() devuelve un puntero,
// strstr da un puntero con la posicion en memoria del caracter, lo
// reemplazamos y esto da el fin de esa linea, REEMPLAZA la cadena original

void parse_req(char *buffer) {

  char *body = strstr(buffer, "\r\n\r\n");

  if (body) {
    *body = '\0';
    body += 4;
  }

  char *request_line = buffer;

  char *next_line = strstr(buffer, "\r\n");

  if (next_line) {
    *next_line = '\0';
    next_line += 2;
  }

  char *method = request_line;
  char *uri = strchr(method, ' ');
  char *version = NULL;

  if (uri) {
    *uri = '\0';
    uri += 1;

    if (version) {
      *version = '\0';
      version += 1;
    }
  }

  printf("Method: %s\n", method);
  printf("URI:     %s\n", uri ? uri : "Not Found");
  printf("Version: %s\n\n", version ? version : "Not Found");

  char *current_line = next_line;
  while (current_line && *current_line != '\0') {

    char *line_end = strstr(current_line, "\r\n");

    if (line_end) {
      *line_end = '\0';
      next_line = line_end + 2;
    } else {
      line_end = NULL;
    }

    char *colon = strchr(current_line, ':');
    if (colon) {
      *colon = '\0';
      char *key = current_line;
      char *value = colon + 1;

      while (*value == ' ') {
        value++;
      }

      printf("key[%s] -> value[%s]", key, value);
    }

    current_line = next_line;
  };
  // Print isolated body
  printf("\n--- BODY ---\n");
  if (body && *body != '\0') {
    printf("%s\n", body);
  } else {
    printf("(Empty Body)\n");
  }
}

void send_fd(int fd) {

  const char *response = "HTTP/1.1 200 OK\r\n"
                         "Content-type: text/plain; charset=utf-8\r\n"
                         "Content-Length: 12\r\n"
                         "\r\n"
                         "Hello World!";
  do {
    // Un proceso similar a read(), hay que poner el send antes del read, para
    // que cada que se accepte un cliente se envie el mensje de bienvenida a
    // traves del socket del cliente
    ssize_t nbytes = send(fd, response, strlen(response), 0);

    if (nbytes > 0) {
      break;
    }

    if (nbytes == -1) {
      if (errno == EINTR) {
        printf("EINTR error!");
        continue;
      } else {
        perror("Error while reading the file descriptor!");
        break;
      }
    }

  } while (1);
}

void read_fd(int fd) {
  // Comprobar el numero de bytes leidos
  // Manejar errores
  // Establecer de manera segura el final de nuestro buffer escrito
  char buffer[1024] = {0};

  do {
    ssize_t nbytes = read(fd, buffer, sizeof(buffer) - 1);

    if (nbytes > 0) {
      buffer[nbytes] = '\0';
      printf("Mensaje: %s\n", buffer);
      continue;
    }

    if (nbytes == 0) {
      printf("End of the file reached!\n");
      break;
    }

    if (nbytes == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        perror("Error while reading the file descriptor!");
        break;
      }
    }

  } while (1);
}
