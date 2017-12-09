#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <termios.h>
#include <fcntl.h>



typedef struct{
  char  user[256];
  char  password[256];
  char  host[256];
  char  file_path[256];
  char  file_name[256];
  char  ip[256];
} url;


int userPassword(url * info_struct, char * complete_url);
int parse_url(char complete_url[],url * info_struct);
int getIp(url* info_struct);
