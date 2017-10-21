#include "data_link.h"

char SET[5] = {FLAG, A, C_SET, A ^ C_SET, FLAG};
char UA[5] = {FLAG, A, C_UA, A ^ C_UA, FLAG};

bool timedOut=false;
int count=0;

void alarmHandler(int sig){
 timedOut=true;
 printf("timed out\n");
 count ++;
}

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

  if(stat == TRANSMITTER)
    if(llopen_transmitter(fd) <0)
      return -1;

  if(stat == RECEIVER)
    if(llopen_receiver(fd) < 0)
      return -1;


  return fd;

}

int set_terminus(int fd){

  struct termios oldtio,newtio;

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
     perror("tcgetattr");
     exit(-1);
   }

   link_layer.portSettings = oldtio;

   bzero(&newtio, sizeof(newtio));
   newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   newtio.c_iflag = IGNPAR;
   newtio.c_oflag = 0;
   newtio.c_lflag = 0;


   if(link_layer.stat == TRANSMITTER)
   newtio.c_cc[VTIME]    = 5;   /* inter-character timer unused */
   newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */



   tcflush(fd, TCIOFLUSH);

   if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
     perror("tcsetattr");
     exit(-1);
   }

   printf("New termios structure set\n");

   return 0;

}

int llopen_transmitter(int fd){


  unsigned char c;
  int state=0;
  bool STOP = false;

  signal(SIGALRM, alarmHandler);

  do{

      printf("llopen: sending SET\n");

      if(write(fd,SET,5) != 5){
          printf("data_link - llopen: error writting SET\n");
          exit(-1);
      }

      timedOut = false;
      alarm(link_layer.timeout);


      while ((!STOP && !timedOut)) {

      if(read(fd,&c,1)==-1){
        printf("data_link - llopen: read error\n");
        exit(-1);
      }

      STOP = updateState(c,&state,UA);

      }

        }while(timedOut && count<link_layer.numTransmissions);

        if(count ==link_layer.numTransmissions)
          return -1;
        else
          return 0;

}

int llopen_receiver(int fd){

    unsigned char c;
    int state =0;
    bool STOP = false;


  while(!STOP ){

    	if (read(fd,&c,1) == -1){
    			printf("data_link - llopen: read error\n");
    		return 1;
     }

     STOP = updateState(c,&state,UA);

    }

    if(write(fd,UA,5) != 5){
        printf("data_link - llopen: error writting UA\n");
        exit(-1);
    }


  return 0;

}

bool updateState(unsigned char c,int* state,char * msg){

  switch (*state) {
      case 0:
        if(c == msg[0])
          *state=1;

          break;

      case 1:
        if(c!= msg[0])
          *state=0;
        if(c == msg[1])
          *state=2;

        break;

      case 2:
        if(c!= msg[0])
          *state =0;
        else
          *state =1;
        if(c == msg[2])
          *state =3;

        break;

      case 3:
        if(c!= msg[0])
          *state =0;
        if( c== msg[0])
          *state =1;
        if( c == (msg[2]^msg[1]))
          *state = 4;

        break;

      case 4:
        if(c != msg[0])
          *state =0;
        else
          return true;

        break;

      default:

        break;

      }


    return false;

}

int llclose(int fd){


    if ( tcsetattr(fd,TCSANOW,&link_layer.portSettings) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);

    return 0;

}
