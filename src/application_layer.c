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
      (char *)malloc(sizeof(char) * (strlen(path) + 1 + strlen(filename)));

  strcpy(full_path, path);
  strcat(full_path, "/");
  strcat(full_path, filename);

  int fd = open(full_path, O_RDONLY);
  if (fd <0) {
    printf("app_layer - send_date: invalid file decriptor\n");
    exit(-1);
   }

  send_start_packet(fd,filename);


}

void send_start_packet(int fd,char* filename){

  struct stat info;
  fstat(fd, &info);

  int filename_len = strlen(filename);
  off_t file_size = info.st_size;


  int start_packet_len = 5 + sizeof(info.st_size) + filename_len;
  char *start_packet = (char *)malloc(sizeof(char) * start_packet_len);

  start_packet[0] = START_BYTE;

  start_packet[1] = FILE_SIZE_BYTE;
  start_packet[2] = sizeof(info.st_size);
  *((off_t *)(start_packet + 3)) = file_size;

  start_packet[3 + sizeof(info.st_size)] = FILE_NAME_BYTE;
  start_packet[4 + sizeof(info.st_size)] = filename_len;
  *((unsigned char *)(start_packet +5+ sizeof(info.st_size))) = *filename;

  llwrite(app_layer.fileDescriptor, start_packet, start_packet_len);


}

void receive_data(){

//  receive_start_packet();
  char packet[MAX_SIZE];
  int packet_length;


  llread(app_layer.fileDescriptor, packet, &packet_length);

}
