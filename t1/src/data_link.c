#include "data_link.h"

char SET[5] = {FLAG, A_SEND, C_SET, A_SEND ^ C_SET, FLAG};
char UA[5] = {FLAG, A_SEND, C_UA, A_SEND ^ C_UA, FLAG};
char DISC[5] ={FLAG, A_SEND, C_DISC, A_SEND ^ C_DISC, FLAG};

bool timedOut=false;
int count=0;
bool ignore_flag= false;

void alarmHandler(int sig){
  timedOut=true;
  count ++;
}


void set_wrong_packets(int numPackets){
  link_layer.wrongPackets = numPackets * FER;
}

void init_link_layer(int timeout,int numTransmissions, int baudRate){

  link_layer.timeout = timeout;
  link_layer.numTransmissions = numTransmissions;
  link_layer.baudRate = baudRate;
  link_layer.wrongPackets = 0;

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



  return 0;

}

int llopen_transmitter(int fd){


  unsigned char c;
  int state=0;

  signal(SIGALRM, alarmHandler);

  do{

    if(write(fd,SET,5) != 5){
      printf("data_link - llopen: error writting SET\n");
      exit(-1);
    }

    timedOut = false;
    alarm(link_layer.timeout);


    while(state!=5 && !timedOut){

      if(read(fd,&c,1)==-1){
        printf("data_link - llopen: read error\n");
        exit(-1);
      }

      state = update_state(c,state,UA);

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

  while(state != 5 ){

    if (read(fd,&c,1) == -1){
      printf("data_link - llopen: read error\n");
      return 1;
    }

    state = update_state(c,state,SET);

  }

  if(write(fd,UA,5) != 5){
    printf("data_link - llopen: error writting UA\n");
    exit(-1);
  }


  return 0;

}

int update_state(unsigned char c,int state,char * msg){



  switch (state) {

    case 0:
    if(c == msg[0])
    return 1;

    break;

    case 1:
    if(c == msg[1])
    return 2;

    if(c!= msg[0])
    return 0;

    break;

    case 2:
    if(c == msg[2])
    return 3;

    if(c!= msg[0])
    return 0;
    else
    return 1;

    break;

    case 3:
    if( c == (msg[2]^msg[1]))
    return 4;

    if(c!= msg[0])
    return 0;
    if( c== msg[0])
    return 1;

    break;

    case 4:
    if(c != msg[0])
    return 0;
    else
    return 5;

    break;

    default:
    break;

  }


  return 0;

}

int llwrite(int fd,  char * packet, int length,int * rej_counter){

  int frame_length;
  unsigned char *frame = create_Iframe(&frame_length, packet, length);

  count=0;
  unsigned char response[255];
  int response_len;



  do{

    if(write_packet(fd,frame,frame_length)<0){
      printf("Failed sending packet.\n");
      return -1;
    }

    timedOut = false;
    alarm(link_layer.timeout);

    while(!timedOut){
        usleep(TIME_WAIT*1000);
    if(read_packet(fd,response,&response_len)==0){

      if(valid_Sframe(response,response_len,RR)){
        //printf("RR%d\n",link_layer.sequenceNumber);
        alarm(0);
        link_layer.sequenceNumber =!link_layer.sequenceNumber;
        return 0;


      }

      if(valid_Sframe(response,response_len,REJ)){
        //printf("REJ%d\n",link_layer.sequenceNumber);
        alarm(0);
        count=0;
        timedOut = true;
          (* rej_counter)++;

        }
      }

    }

  }while(timedOut && count<link_layer.numTransmissions);

  return -1;

}

bool valid_Sframe(unsigned char *response, int response_len, unsigned char C){

  if(response_len <5)
    return false;

  if(response[0]==(unsigned char)FLAG &&
  response[1]==(unsigned char)A_SEND &&
  response[3]==(unsigned char)(response[1]^response[2])&&
  response[4] == (unsigned char)FLAG&&
  ((C== RR  && response[2]==(unsigned char)(!link_layer.sequenceNumber << 7|C)) ||
  (C== REJ && response[2]==(unsigned char)(link_layer.sequenceNumber << 7|C))))
  return true;
  else
  return false;
}

int write_packet(int fd, unsigned char * buffer,int buf_length){
  int total_chars = 0;
  int chars = 0;

  while (total_chars < buf_length) {
    usleep(TIME_WAIT*1000);
    chars = write(fd, buffer, buf_length);

    if( chars <= 0) {
      printf("error in write");
      return -1;
    }

    total_chars += chars;
  }
  return 0;
}

unsigned char *create_Iframe(int *frame_len, char *packet, int packet_len){

  unsigned char *stuff_packet = stuff_frame(packet, &packet_len);

  *frame_len = 5 + packet_len;
  unsigned char *frame = (unsigned char *)malloc(*frame_len * sizeof(char));

  frame[0] = FLAG;
  frame[1] = A_SEND;
  frame[2] = link_layer.sequenceNumber <<6;
  frame[3] = frame[1]^frame[2];

  memcpy(frame + 4, stuff_packet, packet_len);

  frame[*frame_len-1] = FLAG;

  return frame;

}

unsigned char *stuff_frame( char *packet, int *packet_len) {

  unsigned char* stuffed = (unsigned char *)malloc(256 * sizeof(char));

  unsigned char bcc2 = 0;
  int i = 0;
  int j = 0;

  for (i = 0; i < *packet_len; i++)
  bcc2 ^= packet[i];

  if(link_layer.wrongPackets >0){
    bcc2 =0;
    link_layer.wrongPackets--;
  }

  packet[*packet_len]=bcc2;
  *packet_len = *packet_len +1;

  for (i=0; i < *packet_len; i++) {

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
  unsigned char * reply;
  int frame_length;
  int packet_length;

  do{
     if(read_packet(fd, frame, &frame_length)<0){
       printf("data_link - llread: error reading frame\n");
       exit(-1);
     }

  }while(!valid_Iframe(frame));

  unsigned char expected;


  if (frame[frame_length - 3] == ESC)
  expected= frame[frame_length - 2] ^ STUFF_BYTE;
  else
  expected = frame[frame_length - 2];

  // seeting actual packet size
  packet_length = frame_length - HEADER_SIZE;


  if (frame[frame_length - 3] == ESC)
  packet_length--;


  // destuffing frame and update packet value
  unsigned char *destuffed = destuff_frame(frame+4, &packet_length);
  memcpy(packet,destuffed , packet_length);


  // check BB2
  if(validBCC2(packet,packet_length,expected)){


    // check for repeated frames
    if(valid_sequence_number(frame[2])){
      link_layer.sequenceNumber = !link_layer.sequenceNumber;
    }else{
      packet_length=0; // found duplicate

    }
    reply = create_Sframe(RR);

  }else{

    printf("invalid BCC2\n");
        ignore_flag =1;

    // invalid BCC2
    // check for repeated frames
    if (valid_sequence_number(frame[2])){
    reply = create_Sframe(REJ);
    }else
    reply = create_Sframe(RR);


    packet_length =0;

  }

  if(write(fd, reply, S_FRAME_LENGTH) !=  S_FRAME_LENGTH) {
    printf("data_link - llread: write error\n");
    return -1;
  }

  return packet_length;

}

int read_packet(int fd, unsigned char *frame, int *frame_length){

  bool STOP = false;
  char buf;
  *frame_length = 0;
  int flag_count = 0;

  while (!STOP) {
    if (read(fd, &buf, 1) >0) {
      if (buf == FLAG) {
        if(!ignore_flag){
        flag_count++;

        if(flag_count == 2)
        STOP = true;

        frame[*frame_length] = buf;
        (*frame_length)++;
      }else
        ignore_flag= false;

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


bool validBCC2(unsigned char * packet,int packet_length, unsigned char expected){

  unsigned char actual = 0;

  int i;
  for (i = 0; i < packet_length; i++)
  actual ^= packet[i];

  return(actual == expected);

}


bool DISC_frame(unsigned char * reply){

  return(reply[0] == FLAG &&
    reply[1] == ((link_layer.mode == TRANSMITTER) ? A_RECEIVE : A_SEND)&&
    reply[2] == C_DISC &&
    reply[3] == (reply[1] ^ reply[2]) &&
    reply[4] == FLAG);

  }

  bool valid_Iframe(unsigned char * frame){

    if(frame[0] == FLAG &&
      frame[1] == A_SEND &&
      frame[3] == (frame[1] ^ frame[2]))

      return true;

      else
      return false;


    }

  unsigned char * create_Sframe(char control_byte){
      unsigned char * reply =(unsigned char *)malloc(S_FRAME_LENGTH * sizeof(char));

      reply[0] = FLAG;
      reply[1] = A_SEND;
      reply[2] = (link_layer.sequenceNumber << 7) | control_byte;
      reply[3] = reply[1]^reply[2];
      reply[4] = FLAG;



      return reply;
    }

    bool valid_sequence_number(char control_byte) {
      return (control_byte == (link_layer.sequenceNumber << 6));
    }

int llclose(int fd){


if(link_layer.mode == TRANSMITTER)
  if(llclose_transmitter(fd)<0)
    return -1;

if(link_layer.mode == RECEIVER)
  if(llclose_receiver(fd)<0)
    return -1;


if ( tcsetattr(fd,TCSANOW,&link_layer.portSettings) == -1) {
  perror("tcsetattr");
  exit(-1);
}

close(fd);

return 0;

}


int llclose_transmitter(int fd){

  unsigned char c;
  int state =0;

  do{

    if(write(fd,DISC,5) != 5){
      printf("data_link - llclose: error writting DISC\n");
      return -1;
    }

    timedOut = false;
    alarm(link_layer.timeout);


    while(state!=5 && !timedOut){

      if(read(fd,&c,1)==-1){
        printf("data_link - llclose: read error\n");
        return -1;
      }

      state = update_state(c,state,DISC);

    }

  } while(timedOut && count < link_layer.numTransmissions);

  if(write(fd,UA,5) != 5){
    printf("data_link - llclose: error writting UA\n");
    exit(-1);
  }

  return 0;
}

int llclose_receiver(int fd){


    unsigned char c;
    int state =0;

    while(state != 5 ){

      if (read(fd,&c,1) == -1){
        printf("data_link - llopen: read error\n");
        return -1;
      }

      state = update_state(c,state,DISC);

    }

    if(write(fd,DISC,5) != 5){
      printf("data_link - llopen: error writting UA\n");
      return -1;
    }

    state =0;

    while(state != 5 ){

      if (read(fd,&c,1) == -1){
        printf("data_link - llopen: read error\n");
        return -1;
      }

      state = update_state(c,state,UA);

    }


    return 0;

}
