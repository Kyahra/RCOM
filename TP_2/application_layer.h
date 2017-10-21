#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "data_link.h"



typedef struct  {
  int port;
  int fileDescriptor;/*Descritor correspondente à porta série*/
  status stat;/*TRANSMITTER | RECEIVER*/
}applicationLayer;

applicationLayer app_layer;


void set_connection(char * port, char * stat);
