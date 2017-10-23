#define BAUDRATE B38400
#define TIMEOUT 3
#define TRANSMISSIONS 3
#define MAX_SIZE 256

#define COM1 0
#define COM2 1
#define COM1_PORT "/dev/ttyS0"
#define COM2_PORT "/dev/tnt1"

#define FLAG 0x7e
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07

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







typedef enum { TRANSMITTER, RECEIVER } status;
