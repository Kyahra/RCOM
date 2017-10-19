#define ESC  0x7D
#define STUFFING_BYTE 0x20


int packet_lenght
int ignore_flag = 0;


int llread (int fd, char * buffer){

  char *reply;
  int reply_length;

  char frame[512];
  int frame_lenght;
  ignore_flag = 0;
  static int s = 0;

  // VALID frame

  char bcc2;

  if(frame[frame_lenght - 3] == ESC){

      bcc2 = frame[frame_lenght -2] ^ STUFFING_BYTE;

      *packet_lenght = *packet_lenght - 1;

  } else bcc2 = frame[frame_lenght - 2];

  destuff(frame + 4, packet, packet_lenght);  // check function

  /*TO DO*/

}

void destuff(char *packet, char *destuffed, int *packet_lenght) {
  int destuff_iterator = 0;

  int i = 0;

  for (; i < *packet_lenght; i++) {

    if (packet[i] == ESCAPE) {
      destuffed[destuff_iterator] = packet[i + 1] ^ STUFFING_BYTE;
      i++;
    } else
      destuffed[destuff_iterator] = packet[i];
    destuff_iterator++;
  }

  *packet_lenght = destuff_iterator;
}
