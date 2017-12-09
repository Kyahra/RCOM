#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "url_parsing.h"
int main(int argc, char** argv){

  if(argc != 2){
    fprintf(stderr, "Usage: %s <address>\n", argv[0]);
    exit(1);
  }

  url url;
  ftp ftp;

  char ip_address[MAX_STRING_SIZE];
  int port;

  if(parse_url(argv[1], &url) != 0){
    fprintf(stderr, "Invalid URL\n");
    exit(1);
  }

  printf("user:%s\n", url->user);
  printf("pass:%s\n", url->password);
  printf("ip:%s\n", url->ip);
  printf("path:%s\n", url->file_path);
  printf("file_name:%s\n", url->file_name);
  printf("host:%s\n", irl->host);


  initConnection(&ftp,url->ip,url->port,1);
  login(ftp,url);
  activatePassiveMode(ftp,&ip_adress,&port);
  initConnection(&ftp,ip_adress,port,0);
  retrive(ftp, url);
  download(ftp, url);
  endConnection(ftp);

  return 0;
}
