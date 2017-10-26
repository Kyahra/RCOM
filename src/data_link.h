#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"


typedef struct {
  char port[20];
  int baudRate;/*Velocidade de transmissão*/
  unsigned int sequenceNumber;   /*Número de sequência da trama: 0, 1*/
  unsigned int timeout;/*Valor do temporizador: 1 s*/
  unsigned int numTransmissions; /*Número de tentativas em caso defalha*/
  char frame[MAX_SIZE];/*Trama*/
  status mode; /* RECEIVER || TRANSMITTER */
  struct termios portSettings;
}linkLayer;

linkLayer link_layer;



void init_link_layer(int timeout,int numTransmissions, int baudRate);

int set_terminus(int fd);

int llopen(int port,status mode);
int llopen_transmitter(int fd);
int llopen_receiver(int fd);

int llread(int fd, unsigned char *packet);
int llwrite(int fd, char * packet, int length);

char *create_frame(int *frame_len, char *packet, int packet_len);
char *stuff_frame(char *packet, int *packet_len);
unsigned char *destuff_frame(unsigned char *packet,  int *packet_len);
int read_frame(int fd, unsigned char *frame, int *frame_length);
int verify_Sframe(unsigned char *response, int response_len, unsigned char C);


int write_information(int fd, char * buffer,int buf_length);
int read_answer(int fd, char *frame, int *frame_length);


bool validBCC2(unsigned char * packet,unsigned char * frame,int packet_length,int frame_length);
bool valid_frame(unsigned char * frame);
int read_answer(int fd, char *frame, int *frame_length);
bool DISC_frame(unsigned char * reply);

int llclose(int fd);


int write_packet(int fd, char *frame, int frame_length);

bool updateState(unsigned char c,int* state,char * msg);
