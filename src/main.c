#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "application_layer.h"



int main(int argc, char** argv){

   if ( (argc < 3) ||
        ((strcmp(COM1_PORT, argv[1])!=0) && (strcmp(COM2_PORT, argv[1])!=0)) ||
        ((strcmp("T", argv[2]) !=0) &&  (strcmp("R", argv[2]) !=0))) {

     printf("Usage:\tnserial SerialPort ComunicationMode\n\tex: nserial /dev/ttyS1 R\n");
     exit(1);
   }

   init_link_layer(TIMEOUT,TRANSMISSIONS,BAUDRATE);

   set_connection(argv[1],argv[2]);

   char path[] = ".";
   char filename[] = "pinguim.gif";

   if(strcmp(argv[2],"T")==0)
    send_data(path,filename);
   else
    receive_data();

   return 0;
}
