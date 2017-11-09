#define BAUDRATE B115200
#define TIMEOUT 3
#define TRANSMISSIONS 3
#define MAX_SIZE 256

#define COM1 0
#define COM2 1
#define COM1_PORT "/dev/ttyS0"
#define COM2_PORT "/dev/ttyS1"

#define FLAG 0x7e

#define C_SET 0x03
#define C_UA 0x07
#define C_DISC 0x0B

#define DATA_BYTE 1
#define START_BYTE 2
#define END_BYTE 3
#define HEADER_SIZE 6

#define FILE_SIZE_BYTE 0
#define FILE_NAME_BYTE 1


#define ESC 0x7D
#define STUFF_BYTE 0x20

#define PACKET_SIZE 256
#define PACKET_HEADER_SIZE 4
#define DATA_PACKET_SIZE PACKET_SIZE - PACKET_HEADER_SIZE


#define A_SEND 0x03
#define A_RECEIVE 0x01
#define RR 0x05
#define REJ 0x01

#define S_FRAME_LENGTH 5




typedef enum { TRANSMITTER, RECEIVER } status;
