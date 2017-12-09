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
  char * type;
  char * user;
  char * password;
  char * host;
  char * file_path;
  char * file_name;
  char * ip;
} url;


int userPassword(url * info_struct, char * complete_url);
int parse_url(char complete_url[],url * info_struct);
int getIp(url* info_struct);
