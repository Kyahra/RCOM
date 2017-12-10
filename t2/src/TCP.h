#ifndef TCP_H
#define TCP_H
#include "url_parsing.h"

#define CLIENT_CONNECTION_PORT 21
#define MAX_STRING_SIZE 512

int create_connection(char* address, int port);

void login(int control_socket_fd, url* info);

void enter_passive_mode(int sockfd, char* ip, int* port);

void send_retrieve(int control_socket_fd, url* info);

int download_file(int data_socket_fd, url* info);

int close_connection(int control_socket_fd, int data_socket_fd);
#endif
