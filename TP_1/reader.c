/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS0", argv[1])!=0) &&
          (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */



  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) próximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

    char* fr;
    strcpy(buf, "");

    while (STOP==FALSE) {       /* loop for input */
        read(fd,fr,1);   /* returns after 5 chars have been input */
        strcat(buf,fr);               /* so we can printf... */


      if (!strcmp(fr,"\0")) STOP=TRUE;
    }

    printf("%s\n", buf);


    int n = strlen(buf)+1;
    res = write(fd, buf, n);
    printf("%d bytes written\n", res);


  /*
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião
  */



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}


int llopen(int fd) {
    bool end = false;
    int setMsgSize = 5;
    char ua_msg[setMsgSize];

    bzero(ua_msg, setMsgSize);

    ua_msg[0] = FLAG;
    ua_msg[1] = A;
    ua_msg[2] = C_UA;
    ua_msg[3] = A^C_UA;
    ua_msg[4] = FLAG;


        enum State state = S1;

        char buf[1];
        buf[0] = 0;
        int res = -1;
        char received[3];
        int ind = 0;
        while (((res = read(fd,buf,1)) != -1) && end==false) {
            switch (state) {
            case S1:

                if (res != 0 && buf[0] == FLAG) {
                    state = S2;
                }
                break;
            case S2:

                if (res != 0 && buf[0] != FLAG) {
                    state = S3;
                }
                break;
            case S3:

                if (res != 0 && buf[0] == FLAG) {
                    state = END_READ;
                }
                received[ind] = buf[0];

                if (ind == 3) {
                    if (!validBCC(received)) {
                        printf("llopen(): invalid SET\n");
                        return -1;
                    }
                }
                ind++;
                break;
            case END_READ:
                printf("llopen(): received SET\n");
                end =true;
                break;
            }
        }
    printf("sending UA\n");
    write(fd, ua_msg, setMsgSize);
    printf("llopen Success\n");
    return 0;
}
