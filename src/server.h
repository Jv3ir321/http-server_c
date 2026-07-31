#ifndef SERVER_H
#define SERVER_H

#define PORT 8080

void parse_req(char *buffer);

void send_fd(int fd);

void read_fd(int fd);

#endif
