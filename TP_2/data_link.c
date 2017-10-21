#include "data_link.h"


void init_link_layer(int timeout,int numTransmissions, int baudRate){

  link_layer.timeout = timeout;
  link_layer.numTransmissions = numTransmissions;
  link_layer.baudRate = baudRate;

}


int llopen(int port,status stat){

  int fd;

  link_layer.stat = stat;

  switch (port) {
  case COM1:
    strcpy(link_layer.port, COM1_PORT);
    break;

  case COM2:
    strcpy(link_layer.port, COM2_PORT);
    break;

  default:
    printf("data_link - llopen(): invalid port!\n");
    return -1;
  }

  fd = open(link_layer.port, O_RDWR | O_NOCTTY );
  if (fd <0) {perror(link_layer.port); exit(-1); }

  if (set_terminus(fd) != 0) {
    printf("data_link - llopen() - set_terminus: error\n");
    return -1;
  }



  return fd;

}

int set_terminus(int fd){

  struct termios oldtio,newtio;

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
     perror("tcgetattr");
     exit(-1);
   }

   bzero(&newtio, sizeof(newtio));
   newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   newtio.c_iflag = IGNPAR;
   newtio.c_oflag = 0;
   newtio.c_lflag = 0;

   newtio.c_cc[VTIME]    = 5;   /* inter-character timer unused */
   newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */


   tcflush(fd, TCIOFLUSH);

   if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
     perror("tcsetattr");
     exit(-1);
   }

   return 0;

}
