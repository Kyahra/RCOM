#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#include "utils.h"


typedef struct {
  char port[20];
  int baudRate;/*Velocidade de transmissão*/
  unsigned int sequenceNumber;   /*Número de sequência da trama: 0, 1*/
  unsigned int timeout;/*Valor do temporizador: 1 s*/
  unsigned int numTransmissions; /*Número de tentativas em caso defalha*/
  char frame[MAX_SIZE];/*Trama*/
  status stat;
  struct termios portSettings;
}linkLayer;

linkLayer link_layer;



void init_link_layer(int timeout,int numTransmissions, int baudRate);

int set_terminus(int fd);

int llopen(int port,status stat);
int llopen_transmitter(int fd);
int llopen_receiver(int fd);

int llwrite(int fd, char * packet, int length);
int llread(int fd, char *packet, int *packet_len);

int llclose(int fd);

char *create_frame(int *frame_len, char *packet, int packet_len);
char *stuff_frame(char *packet, int *packet_len);

int read_frame(int fd, char *frame, int *frame_length);

bool updateState(unsigned char c,int* state,char * msg);
