#include "application_layer.h"



void set_connection(char * port, char * stat){
  
  if(strcmp(port,COM1_PORT)==0)
     app_layer.port = COM1;

  if(strcmp(port,COM2_PORT)== 0)
     app_layer.port =COM2;

  if(strcmp(stat,"T")==0){
    app_layer.stat = TRANSMITTER;
    printf("TRANSMITTER\n");
  }

  if(strcmp(stat,"R")== 0){
    app_layer.stat = RECEIVER;
    printf("RECEIVER\n");
  }

  app_layer.fileDescriptor = llopen(app_layer.port,app_layer.stat);

  if(app_layer.fileDescriptor < 0){
      printf("app_layer - set_connection(): invalid file descriptor\n");
      exit(-1);
  }

  llclose(app_layer.fileDescriptor);



}
