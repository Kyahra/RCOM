#include "data_link.h"

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
    if(llopen_transmitter(fd) <0){
      printf("data_link - llopen: failed");
    }

  if(stat == RECEIVER)
    llopen_receiver(fd);



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


   if(link_layer.status == TRANSMITTER)
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

int llclose(int fd){


    if ( tcsetattr(fd,TCSANOW,&link_layer.portSettings) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);

    return 0;

}

int llopen_transmitter(int fd){

  char SET[5] = {FLAG, A, C_SET, A ^ C_SET, FLAG};
  bool STOP = false;
  unsigned char c;
  int state=0;

  signal(SIGALRM, alarmHandler);

  do{

      printf("llopen: sending SET\n");

      if(write(fd,SET,5) != 5){
          printf("data_link - llopen: write error\n");
          exit(-1);
      }

      timedOut = false;
      alarm(3);


      while ((!STOP && !timedOut)) {

      if(read(fd,&c,1)==-1){
        printf("data_link - llopen: write error\n");
        exit(-1);
      }

      printf("state: %d\n",state);
      printf("char: %04x\n",c);


        switch (state) {
            case 0:
              if(c == FLAG)
                state=1;
            break;

            case 1:
              if(c!= FLAG)
                state=0;
              if(c == A)
                state=2;
              break;

            case 2:
              if(c!= FLAG)
                state =0;
              else
                state =1;

              if(c == C_UA)
                state =3;
              break;

            case 3:

              if(c!= FLAG)
                state =0;

              if( c== FLAG)
                state =1;

              if( c == (C_UA^A))
                state = 4;
              break;

            case 4:
              if(c != FLAG)
                state =0;

              if(c== FLAG){
                STOP = true;
                printf("data-link - llopen(): UA received\n");
              }
              break;

            default:
              break;

            }
          }
        }while(timedOut && count<3);

        if(count ==3)
          return -1;
        else
          return 0;

}

int llopen_receiver(int fd){

    char UA[5] = {FLAG, A, C_UA, A ^ C_UA, FLAG};
    unsigned char c;
    bool STOP = false;
    int state =0;


  while(!STOP ){

    	if (read(fd,&c,1) == -1){
    			printf("ERROR in read()");
    		return 1;
     }

     printf("char = %04x\n", c);
     printf("state = %d\n", state);

      switch(state){
      case 0:
        if(c == FLAG)
          state =1;
        break;
      case 1:
        if(c!= FLAG)
          state =0;
        if(c ==A  )
          state = 2;
        break;
      case 2:
        if(c!= FLAG)
          state =0;
        if(c == FLAG)
          state =1;
        if(c == C_SET )
          state =3;
        break;
      case 3:
        if(c!= FLAG)
          state =0;
        if( c== FLAG)
          state =1;
        if( c == (C_SET^A))
          state = 4;
        break;
      case 4:
        if(c != FLAG)
          state =0;
        if(c== FLAG ){
            STOP = true;
            printf("llopen(): received SET\n");
        }
      break;
      }

    }

    write(fd, UA, 5);

  return 0;

}
