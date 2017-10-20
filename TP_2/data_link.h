
#define MAX_SIZE 10000

struct linkLayer {
  char port[20];/*Dispositivo /dev/ttySx, x = 0, 1*/
  int baudRate;/*Velocidade de transmissão*/
  unsigned int sequenceNumber;   /*Número de sequência da trama: 0, 1*/
  unsigned int timeout;/*Valor do temporizador: 1 s*/
  unsigned int numTransmissions; /*Número de tentativas em caso defalha*/
  char frame[MAX_SIZE];/*Trama*/
}

linkLayer link_layer;

int init_link_layer(int timeout,int numTransmissions, int baudRate);
int llopen(int fd, char * mode);
