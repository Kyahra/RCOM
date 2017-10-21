#d*/efine ESC  0x7D
#define STUFFING_BYTE 0x20


int packet_lenght;
int ignore_flag = 0;


int llread (int fd, char * buffer){

	/*
  char *reply;
  int reply_length;

  char frame[512];
  int frame_lenght;
  ignore_flag = 0;
  static int s = 0;

  // VALID frame

  char expected_bcc2;

  if(frame[frame_lenght - 3] == ESC){

	  expected_bcc2 = frame[frame_lenght -2] ^ STUFFING_BYTE;

      *packet_lenght = *packet_lenght - 1;

  } else expected_bcc2 = frame[frame_lenght - 2];

  destuff(frame + 4, packet, packet_lenght);  // check function

  */
   

	int ret, sizeAfterDestuffing;

	readingFrame(fd, frame);

	sizeAfterDestuffing = destuffingFrame(frame);

	// Processing frame
	if (frame[FIELD_CONTROL] == NUMBER_OF_SEQUENCE_0 || frame[FIELD_CONTROL] == NUMBER_OF_SEQUENCE_1) {
		ret = processingDataFrame(frame);
	}

	if (ret == 0) {
		ret = sizeAfterDestuffing;
	}

	return ret;


}

int readingFrame(int fd, unsigned char *frame) {
	
	unsigned char oneByte;
	ReadingArrayState state = START;
	int over = 0;
	int i = 0;

	(void)signal(SIGALRM, timeout);

	while (!over) {
		alarm(timeoutTime);
		read(fd, &oneByte, 1);
		alarm(timeoutTime);

		switch (state) {
		case START:
			if (oneByte == FLAG) {
				frame[i] = oneByte;
				i++;
				state = FLAG_STATE;
			}
			break;
		case FLAG_STATE:
			if (oneByte != FLAG) {
				frame[i] = oneByte;
				i++;
				state = A_STATE;
			}
			break;
		case A_STATE:
			if (oneByte != FLAG) {
				frame[i] = oneByte;
				i++;
				state = C_STATE;
			}
			break;
		case C_STATE:
			if (oneByte != FLAG) {
				frame[i] = oneByte;
				i++;
				state = BCC;
			}
			break;
		case BCC:
			if (oneByte != FLAG) {
				frame[i] = oneByte;
				i++;
			}
			else if (oneByte == FLAG) {
				frame[i] = oneByte;
				i++;
				over = 1;
			}
			break;
		case SUCCESS:
		default:
			break;
		}
	}

	return i; // returning the size of the frame
}


ReadingArrayState nextState(ReadingArrayState state) {
	switch (state) {
	case START:
		state = FLAG_STATE;
		break;
	case FLAG_STATE:
		state = A_STATE;
		break;
	case A_STATE:
		state = C_STATE;
		break;
	case C_STATE:
		state = BCC;
		break;
	case BCC:
		state = SUCCESS;
		break;
	case SUCCESS:
		break;
	}

	return state;
}


void destuff(char *packet, char *destuffed, int *packet_lenght) {
  
  int iterator = 0, i = 0;

  for (; i < *packet_lenght; i++) {

    if (packet[i] == ESCAPE) {
      destuffed[iterator] = packet[i + 1] ^ STUFFING_BYTE;
      i++;
    } else
		destuffed[iterator] = packet[i];
	iterator++;
  }

  *packet_lenght = iterator;

}

int read_from_tty(int fd, char *frame, int *frame_lenght) {
	
	int read_chars = 0;
	char buf;
	*frame_lenght = 0;
	int initial_flag = 0;

	STOP = 0;
	while (!STOP) {                   /* loop for input */
		read_chars = read(fd, &buf, 1); /* returns after x chars have been input */

		if (read_chars > 0) { // If characters were read
			if (buf == FLAG) { //If the char is a FLAG
							   //Set frame start to true.
				if (!ignore_flag) {
					initial_flag = !initial_flag;

					//If it is the second flag, then the frame
					//has ended
					if (!initial_flag)
						STOP = 1;

					frame[*frame_lenght] = buf;
					(*frame_lenght)++;
				}
				else
					ignore_flag = 0;
			}
			else {
				//If the char is not a flag and
				//the final flag has not been found
				//then add it to the frame.
				if (initial_flag) {
					frame[*frame_lenght] = buf;
					(*frame_lenght)++;
				}
			}
		}
		else {// If no characters were read or there was an error
			  //printf("Error!!! %s\n", strerror(errno));
			return -1;
		}
	}

	return 0;
}
