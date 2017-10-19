#define FLAG 0x7e
#define ESC 0x7D
#define ESC_HIDE_BYTE 0x5D
#define FLAG_HIDE_BYTE 0x5E

unsigned char frame[255];


unsigned char BCC2(unsigned char *frame, unsigned int length) {
  unsigned char BCC = 0;

  unsigned int i = 0;
  for (; i < length; i++) {
    BCC ^= frame[i];
  }

  return BCC;
}
int llwrite(int fd, char* buffer, int length){

  int sequence= buffer[length-1];
  int rejections_number=0;

  frame[0]=FLAG;
  frame[1]=A;
  frame[2]=sequence;
  frame[3]=frame[1]^frame[2];

  for(int i=0; i<length-1;i++){
    frame[i + 4] = buffer[i];
  }

  //ou exclusivo do d1 a dn
  frame[length +4]=BCC2(buffer, length);
  frame[length + 5] = FLAG;

 }

 int stuff_frame(unsigned char *frame, int frame_size){


      for(int i=1; i<frame_size-1;i++){

       if(frame[i]== FLAG){
        frame[i]=ESC;
        i++;
        //TODO:shift da trama
        frame_size++;
        frame[i]=FLAG_HIDE_BYTE;
      }

        if(frame[i]==ESC){
          i++;
          //TODO:shift da trama
          frame_size++;
          frame[i] = ESC_HIDE_BYTE;
        }

    return frame_size;

      }
 }
