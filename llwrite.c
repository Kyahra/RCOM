#define FLAG 0x7e
#define ESC 0x7D
#define ESC_HIDE_BYTE 0x5D
#define FLAG_HIDE_BYTE 0x5E
#define C_RR0 0x05
#define C_RR1 0x85
#define C_REJ 0x01

unsigned char frame[255];
int frame_size = 0;


unsigned char BCC2(unsigned char *frame, unsigned int length) {
  unsigned char BCC = 0;

  unsigned int i = 0;
  for (; i < length; i++) {
    BCC ^= frame[i];
  }

  return BCC;
}

int shift_right(unsigned char *frame, int initialeminem_pos, int frame_size){

     for(; initial_pos<= frame_size-1;initial_pos--){
       frame[frame_size+1] = frame[frame_size];
     }


     return 0;
}

int stuff_frame(unsigned char *frame, int frame_size){


     for(int i=1; i<frame_size-1;i++){

      if(frame[i]== FLAG){
       frame[i]=ESC;
       i++;
       shift_right(frame,i,frame_size);
       frame_size++;
       frame[i]=FLAG_HIDE_BYTE;
     }

       if(frame[i]==ESC){
         i++;
         shift_right(frame,i,frame_size);
         frame_size++;
         frame[i] = ESC_HIDE_BYTE;
       }

   return frame_size;

     }
}

int create_I_frame(char * buffer, int length){

  static char c = 1;
  c = !c;
  frame[0]=FLAG;
  frame[1]=A;
  frame[2]=c << 6;;
  frame[3]=frame[1]^frame[2];

  for(int i=0; i<length-1;i++){
    frame[i + 4] = buffer[i];
  }

  //ou exclusivo do d1 a dn
  frame[length +4]=BCC2(buffer, length);
  frame[length + 5] = FLAG;
  frame_size = stuff_frame(frame, length + 6);

  return 0;

}



int llwrite(int fd, char* buffer, int length){

  int rejections_number=0;
  char temp[5];
  bool STOP = false;
  unsigned char c;

  create_I_frame(buffer,length);

  int i=0;

  do{



  }
while(timedOut && count<=3);




 }
