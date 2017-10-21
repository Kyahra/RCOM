#define BAUDRATE B38400
#define TIMEOUT 3
#define TRANSMISSIONS 3
#define MAX_SIZE 10000

#define COM1 0
#define COM2 1
#define COM1_PORT "/dev/tnt0"
#define COM2_PORT "/dev/tnt1"

#define FLAG 0x7e
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07

typedef enum { TRANSMITTER, RECEIVER } status;
