#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "data_link.h"



typedef struct  {
  int port;
  int fileDescriptor;/*Descritor correspondente à porta série*/
  status mode;/*TRANSMITTER | RECEIVER*/
}applicationLayer;

applicationLayer app_layer;

void set_connection(char * port, char * mode);

void send_data(char * path, char* filename);
void receive_data();

void send_start_packet(int fd, char* filename);
char* receive_start_packet(off_t* file_size);
