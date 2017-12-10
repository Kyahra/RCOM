#ifndef TCP_H
#define TCP_H
#include "url_parsing.h"


#define MAX_SIZE 516

typedef struct
{
    int ctrl_socket_fd; // file descriptor to control socket
    int data_socket_fd; // file descriptor to data socket
} ftp;

int initConnection(ftp* ftp,char* address, int port);
int endConnection(ftp ftp);

int initSocket(char* ip_address,int port);
int socketRead(int socketfd, char* repply);
int socketWrite(int socketfd, char* cmd);

void login(ftp ftp, url url);
void passiveMode(ftp ftp, char* ip_adress, int* port);
void retrieve(ftp ftp, url url);
int download(ftp ftp, url url);


#endif
