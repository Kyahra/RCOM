#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "url_parsing.h"
#include "TCP.h"

int main(int argc, char** argv){

  if(argc != 2){
    fprintf(stderr, "Usage: %s <address>\n", argv[0]);
    exit(1);
  }

  url url;

  if(parse_url(argv[1], &url) != 0){
    fprintf(stderr, "Invalid URL\n");
    exit(1);
  }

  printf("user:%s\n", url.user);
  printf("pass:%s\n", url.password);
  printf("ip:%s\n", url.ip);
  printf("path:%s\n", url.file_path);
  printf("file_name:%s\n", url.file_name);
  printf("host:%s\n", url.host);



    int control_socket_fd;
    if((control_socket_fd = create_connection(url.ip, CLIENT_CONNECTION_PORT)) == 0){
      fprintf(stderr, "Error opening control connection\n");
      exit(1);
    }

    login(control_socket_fd, &url);
    char data_address[MAX_STRING_SIZE];
    int port;
    enter_passive_mode(control_socket_fd, data_address, &port);

    int data_socket_fd;
    if((data_socket_fd = create_connection(data_address, port)) == 0){
      fprintf(stderr, "Error opening data connection\n");
      exit(1);
    }
    send_retrieve(control_socket_fd, &url);
    download_file(data_socket_fd, &url);
    close_connection(control_socket_fd, data_socket_fd);

    return 0;
  }
