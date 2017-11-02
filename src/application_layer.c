

#include "application_layer.h"


void set_connection(char * port, char * stat){

  if(strcmp(port,COM1_PORT)==0)
  app_layer.port = COM1;

  if(strcmp(port,COM2_PORT)== 0)
  app_layer.port =COM2;

  if(strcmp(stat,"T")==0){
    app_layer.mode = TRANSMITTER;
    printf("TRANSMITTER\n");
  }

  if(strcmp(stat,"R")== 0){
    app_layer.mode = RECEIVER;
    printf("RECEIVER\n");
  }

  app_layer.fileDescriptor = llopen(app_layer.port,app_layer.mode);

  if(app_layer.fileDescriptor< 0){
    printf("app_layer - set_connection(): invalid file descriptor\n");
    exit(-1);
  }


}

void send_data(char * path, char* filename){
  char *full_path =
  ( char *)malloc(sizeof(char) * (strlen(path) + 1 + strlen(filename)));

  strcpy(full_path, path);
  strcat(full_path, "/");
  strcat(full_path, filename);

  int fd = open(full_path, O_RDONLY);
  if (fd <0) {
    printf("app_layer - send_data: invalid file decriptor\n");
    exit(-1);
  }

  send_control_packet(fd,filename, START_BYTE);
  send_packets(fd,filename);
  send_control_packet(fd,filename, END_BYTE);


  llclose(app_layer.fileDescriptor);


}

void send_packets(int fd, char* filename){

  struct stat info;
  fstat(fd, &info);

  off_t file_size = info.st_size;


  char data[DATA_PACKET_SIZE];
  int i = 0;
  off_t bytes_to_read = file_size;

  while(bytes_to_read>0){
    int num_chars=read(fd,data,DATA_PACKET_SIZE);

    if(num_chars <0){
      printf("app_layer - send_packets: error reading data\n" );
      exit(-1);
    }

    char data_packet[PACKET_SIZE];
    int packet_size= num_chars + PACKET_HEADER_SIZE;

    data_packet[0] = DATA_BYTE;
    data_packet[1] = i % 256;
    data_packet[2] = 0;
    data_packet[3] = num_chars;

    memcpy(data_packet +PACKET_HEADER_SIZE,data,num_chars);

    if(llwrite(app_layer.fileDescriptor,data_packet,packet_size)==-1)
    exit(1);
    bytes_to_read -= num_chars;
    i++;

  }



}

void send_control_packet(int fd, char* filename,unsigned char control_byte){

  struct stat info;
  fstat(fd, &info);

  int filename_len = strlen(filename);
  off_t file_size = info.st_size;


  int start_packet_len = 5 + sizeof(info.st_size) + filename_len;
  char *start_packet = ( char *)malloc(sizeof(char) * start_packet_len);

  start_packet[0] = control_byte;

  start_packet[1] = FILE_SIZE_BYTE;
  start_packet[2] = sizeof(info.st_size);
  *((off_t *)(start_packet + 3)) = file_size;

  start_packet[3 + sizeof(info.st_size)] = FILE_NAME_BYTE;
  start_packet[4 + sizeof(info.st_size)] = filename_len;
  strcat(start_packet + 5 + sizeof(info.st_size),filename);

  llwrite(app_layer.fileDescriptor, start_packet, start_packet_len);

}

void receive_data(){

  // receive start packet

  off_t file_size;
  char* file_name;

  file_name = receive_start_packet(&file_size);

  strcpy(file_name, "p.gif");

  int fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC);

  if (fd <0) {
    printf("app_layer - receive_data: invalid file descriptor\n");
    exit(-1);
  }


  unsigned char packet[PACKET_SIZE];
  int packet_length;

  while(true){

    do{

      packet_length = llread(app_layer.fileDescriptor, packet);

    }while(packet_length ==0);


    if (packet_length < 0) {
      printf("app_layer - receive_data: error llread\n");
      close(fd);
      exit(-1);
    }


    if(packet[0] == END_BYTE)
    break;



    unsigned int data_len = packet[2] * 256 + packet[3];


    if (write(fd, packet + 4, data_len) != data_len) {
      printf("app_layer - receive_data: write error\n");
      close(fd);
      exit(-1);
    }

  }

  llclose(app_layer.fileDescriptor);

  close(fd);

}

char* receive_start_packet(off_t* file_size){

  unsigned char packet[PACKET_SIZE];
  int packet_length;


  do {

    packet_length = llread(app_layer.fileDescriptor, packet);

    if ( packet_length <0) {
      printf("app_layer - receive_data - receive_start_packet: error.\n");
      exit(-1);
    }

  }while(packet[0] != (unsigned char)START_BYTE || packet_length==0);



  int i;
  // get file size
  i = 1;
  while (i < packet_length) {
    if (packet[i] == FILE_SIZE_BYTE){
      *file_size = *((off_t *)(packet + i + 2));
      break;
    }

    i += 2 + packet[i + 1];
  }



  // get file name
  i = 1;
  while (i < packet_length) {
    if (packet[i] == FILE_NAME_BYTE) {
      char * file_name = (char *)malloc((packet[i + 1] + 1) * sizeof(char));
      memcpy(file_name, packet + i + 2, packet[i + 1]);
      file_name[(packet[i + 1] + 1)] = 0;
      return file_name;
    }

    i += 2 + packet[i + 1];
  }


  return NULL;

}
