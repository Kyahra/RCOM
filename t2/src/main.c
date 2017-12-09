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

  url info;
  if(parse_url(argv[1], &info) != 0){
    fprintf(stderr, "Invalid URL\n");
    exit(1);
  }

  printf("user:%s\n", info->user);
  printf("pass:%s\n", info->password);
  printf("ip:%s\n", info->ip);
  printf("path:%s\n", info->file_path);
  printf("file_name:%s\n", info->file_name);
  printf("host:%s\n", info->host);


return 0;
}
