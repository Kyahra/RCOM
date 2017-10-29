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
  status mode; /* RECEIVER || TRANSMITTER */
  struct termios portSettings;
}linkLayer;

linkLayer link_layer;



void init_link_layer(int timeout,int numTransmissions, int baudRate);

int set_terminus(int fd);

int update_state(unsigned char c,int state,char * msg);

int llopen(int port,status mode);
int llopen_transmitter(int fd);
int llopen_receiver(int fd);

int llread(int fd, unsigned char *packet);
int llwrite(int fd,  char * packet, int length);

int llclose(int fd);
int llclose_transmitter(int fd);
int llclose_receiver(int fd);

unsigned char *create_Iframe(int *frame_len, char *packet, int packet_len);
unsigned char * create_Sframe(char control_byte);

unsigned char *stuff_frame( char *packet, int *packet_len);
unsigned char *destuff_frame(unsigned char *packet,  int *packet_len);

int read_packet(int fd, unsigned char *frame, int *frame_length);
int write_packet(int fd, unsigned char * buffer,int buf_length);


bool valid_Iframe(unsigned char * frame);
bool valid_sequence_number(char control_byte);
bool validBCC2(unsigned char * packet,int packet_length,unsigned char expected);
bool valid_Sframe(unsigned char *response, int response_len, unsigned char C);

bool DISC_frame(unsigned char * reply);
