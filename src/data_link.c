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

int llopen(int port,status mode){

  int fd;

  link_layer.mode = mode;

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
	
  link_layer.mode = mode;

  if(mode == TRANSMITTER)
    if(llopen_transmitter(fd) <0)
      return -1;

  if(mode == RECEIVER)
    if(llopen_receiver(fd) < 0)
      return -1;

  link_layer.sequenceNumber =0;

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


   if(link_layer.mode == TRANSMITTER){
     newtio.c_cc[VTIME]    = 5;   /* inter-character timer unused */
     newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
   }else{
     newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
     newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */
   }


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

  } while(timedOut && count < link_layer.numTransmissions);

  if(count == link_layer.numTransmissions)
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

     STOP = updateState(c,&state,SET);

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

int llwrite(int fd, char * packet, int length){

  int frame_length;

  char *frame = create_frame(&frame_length, packet, length);


  //unsigned char reply[255];
    //int reply_length;
    count=0;



    do{

       if(write_information(fd,frame,frame_length)<0){
         printf("Failed sending packet.\n");
  return -1;
       }
       timedOut = false;
       alarm(link_layer.timeout);






}
while(timedOut && count<link_layer.numTransmissions);

   return 0;
}


int read_answer(int fd, char *frame, int *frame_length){

	
}



int write_information(int fd, char * buffer,int buf_length){
  int total_chars = 0;
   int chars = 0;

   while (total_chars < buf_length) {
      chars = write(fd, buffer, buf_length);

     if( chars <= 0) {
       printf("error in write");
       return -1;
     }

     total_chars += chars;
   }

   return 0;
 }


char *create_frame(int *frame_len, char *packet, int packet_len){

  unsigned char bcc2 = 0;

  int i;
  for (i = 0; i < packet_len; i++)
    bcc2 ^= packet[i];

  packet[packet_len]=bcc2;
  packet_len++;

  char *stuff_packet = stuff_frame(packet, &packet_len);

  *frame_len = 5 + packet_len;
  char *frame = (char *)malloc(*frame_len * sizeof(char));

  frame[0] = FLAG;
  frame[1] = A;
  frame[2] = link_layer.sequenceNumber;
  frame[3] = frame[1]^frame[2];

  memcpy(frame + 4, stuff_packet, packet_len);

  frame[*frame_len-1] = FLAG;

  return frame;

}

char *stuff_frame(char *packet, int *packet_len) {

  char *stuffed = (char *)malloc(((*packet_len) + 256) * sizeof(char));

  int i = 0;
  int j = 0;

  for (; i < *packet_len; i++) {

    if (packet[i] == ESC || packet[i] == FLAG) {

      stuffed[j] = ESC;
      stuffed[++j] = packet[i] ^ STUFF_BYTE;

    } else
      stuffed[j] = packet[i];

    j++;
  }

  *packet_len = j;
  return stuffed;
}

int llread(int fd, unsigned char *packet) {

  unsigned char frame[MAX_SIZE];
  int frame_length;
  int packet_length;
  // int reply_length;
  // char *reply;


  if(read_frame(fd, frame, &frame_length)<0){
    printf("data_link - llread: error reading frame\n");
    exit(-1);
  }

  if(!valid_frame(frame)){
		printf("yo\n");
	}


  packet_length = frame_length - HEADER_SIZE;
	
  if (frame[frame_length - 3] == ESC)
        packet_length--;
  
  unsigned char *destuffed = destuff_frame(frame+4, &packet_length);

  memcpy(packet,destuffed , packet_length);

  if(!validBCC2(packet,frame,packet_length,frame_length))
	printf("bcc2 is not valid\n");

  return packet_length;

}

int read_frame(int fd, unsigned char *frame, int *frame_length){

  bool STOP = false;
  char buf;
  *frame_length = 0;
  int flag_count = 0;

  while (!STOP) {
    if (read(fd, &buf, 1) >0) {
      if (buf == FLAG) {
        flag_count++;

        if(flag_count == 2)
          STOP = true;

        frame[*frame_length] = buf;
        (*frame_length)++;

        }else {
        if(flag_count>0) {
          frame[*frame_length] = buf;
          (*frame_length)++;
          }
        }
      }else
    return -1;
  }

  return 0;
}


unsigned char *destuff_frame(unsigned char *packet,  int *packet_len){

  unsigned char *destuffed = (unsigned char *)malloc(((*packet_len)) * sizeof(unsigned char));
  int i = 0;
  int j = 0;

  for (; i < *packet_len; i++) {
    if (packet[i] == ESC) {
      destuffed[j] = packet[i + 1] ^ STUFF_BYTE;
      i++;
    } else
      destuffed[j] = packet[i];

    j++;
  }

  *packet_len = j;

  return destuffed;
}

int llclose(int fd){

	

    if ( tcsetattr(fd,TCSANOW,&link_layer.portSettings) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);

    return 0;

}

bool validBCC2(unsigned char * packet,unsigned char * frame,int packet_length,int frame_length){

	char expected;
 	char actual = 0;

      if (frame[frame_length - 3] == ESC) 
        expected= frame[frame_length - 2] ^ STUFF_BYTE;
      else
		expected = frame[frame_length - 2];

     int i;
      for (i = 0; i < packet_length; i++)
        actual ^= packet[i];

      return(actual == expected);


}

int write_packet(int fd, char *frame, int frame_length){

  int total_chars = 0;
  int written_chars = 0;

  while(total_chars < frame_length){

    written_chars = write(fd, frame, frame_length);

    if(written_chars <= 0){

      printf("Written chars: %d\n", written_chars);
      printf("%s\n", strerror(errno));
      return -1;
    }

    total_chars += written_chars;
  }

  return 0;
}

 bool DISC_frame(unsigned char * reply){

   return(reply[0] == FLAG &&
      reply[1] == ((link_layer.mode == TRANSMITTER) ? RECEIVE : SEND)&&
      reply[2] == DISC && 
	  reply[3] == (reply[1] ^ reply[2]) &&
      reply[4] == FLAG);
     

 }

bool valid_frame(unsigned char * frame){


   if(frame[0] == FLAG && 
	  frame[1] == SEND && 
	  frame[3] == (frame[1] ^ frame[2]))
     return true;
   else 
	return false;


 }
//
//
// char *create_frame_US(int *frame_length, int control_byte) {
//
//   static char r = 0;
//   char *buf = (char *)malloc(US_FRAME_LENGTH * sizeof(char));
//   buf[0] = FLAG;
//
//   if(data_link.stat == TRANSMITTER) {
//
//     if(control_byte == SET || control_byte == DISC)
//       buf[1] = SEND;
//     else
//       buf[1] = RECEIVE;
//   }
//   else {
//
//     if(control_byte == RR || control_byte == REJ || control_byte == UA)
//       buf[1] = SEND;
//     else
//       buf[1] = RECEIVE;
//   }
//
//   if(control_byte == RR ||  control_byte == REJ) {
//
//     buf[2] = r << 7 | control_byte;
//     r = !r;
//   }
//   else buf[2] = control_byte;
//
//   buf[3] = buf[1] ^ buf[2];
//   buf[4] = FLAG;
//   *frame_length = US_FRAME_LENGTH;
//
//   return buf;
//
// }

// int UA_frame(char *reply) {
//
//   if(reply[0] == FLAG &&
//      reply[1] == ((data_link.stat == TRANSMITTER) ? SEND : RECEIVE) &&
//      reply[2] == UA && reply[3] == (reply[1] ^ reply[2]) &&
//      reply[4] == FLAG)
//     return 1;
//   else return 0;
// }





//
// int valid_seq_number(char control_byte, int s) {
//
//   return (control_byte == (s << 6));
// }


// int close_connection(int fd) {
//
//   int frame_length = 0;
//   char *frame = create_frame_US(&frame_length, DISC);
//
//    TODO: send_frame_US
//   if(send_frame_US(fd, frame, frame_length, UA_frame) != 0){
//     printf("Not able to send frame on llclose().\n");
//     return -1;
//   }
//
//   return 0;
//
// }
