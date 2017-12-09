#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>

#define PORT 21
#define MAX_STRING_SIZE 516

typedef struct{
  int ctrl_socket_fd;
  int data_socket_fd;
} ftp;


int initConnection(char * ip_adress,int port);
int endConnection(ftp ftp);

int login(ftp ftp, url url);

int socketRead(int socketfd, char* str);
int socketWrite(int socketfd, char* cmd);

int activatePassiveMode(ftp ftp,char* ip_adress,int* port);
void retrive(ftp ftp, url url);
int endConnection(ftp ftp);
