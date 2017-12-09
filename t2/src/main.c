#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "url_parsing.c"
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

return 0;
}
