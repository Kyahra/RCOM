#include "TPC.h"

int initConnection(ftp * ftp,char * ip_adress,int port, int type){

	struct	sockaddr_in server_addr;

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_adress);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */

	/*open an TCP socket*/

  if(type ==1){
	if ((ftp->ctrl_socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}
	/*connect to the server*/
    	if(connect(ftp->ctrl_socket_fd,
	           (struct sockaddr *)&server_addr,
		   sizeof(server_addr)) < 0){
        	perror("connect()");

    }else{
      if ((ftp->data_socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
            perror("socket()");
              exit(0);
          }
      /*connect to the server*/
          if(connect(ftp->data_socket_fd,
                 (struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0){
              perror("connect()");

    }
		exit(0);
	}


  return sockfd;
}

}


int login(ftp ftp, url url){

  char username_cmd[MAX_STRING_SIZE];
  char password_cmd[MAX_STRING_SIZE];

  socketRead(ftp.ctrl_socket_fd, NULL);

  sprintf(username_cmd, "USER %s\r\n", url.user);
  socketWrite(ftp.ctrl_socket_fd, username_cmd);
  socketRead(ftp.ctrl_socket_fd, NULL);
  sprintf(password_cmd, "PASS %s\r\n", url.password);
  if(socketWrite(ftp.ctrl_socket_fd, password_cmd) != 0){
      fprintf(stderr, "Bad login. Exiting...\n");
      exit(1);
  }
  socketRead(ftp.ctrl_socket_fd, NULL);

}


int socketRead(int socketfd, char* str){

  FILE* fp = fdopen(socketfd, "r");

	do {
		memset(str, 0, MAX_STRING_SIZE);
		str = fgets(str, MAX_STRING_SIZE, fp);
		printf("%s", str);
	} while (!('1' <= str[0] && str[0] <= '5') || str[3] != ' ');

  return 0;

}


int socketWrite(int socketfd, char* cmd){
    int ret = write(socketfd, cmd, strlen(cmd));
    return ret;
}

int activatePassiveMode(ftp ftp,char* ip_adress,int* port){
  char repply[MAX_STRING_SIZE];

  if(socketWrite(ftp.ctrl_socket_fd, "PASV\r\n") != 0){
    fprintf(stderr, "Error entering passive mode. Exiting...\n");
    exit(1);
  }

  socketRead(ftp.ctrl_socket_fd, repply);

  int values[6];
  char* data = strchr(repply, '(');
  sscanf(data, "(%d, %d, %d, %d, %d, %d)", &values[0],&values[1],&values[2],&values[3],&values[4],&values[5]);
  sprintf(ip_adress, "%d.%d.%d.%d", values[0],values[1],values[2],values[3]);
  * port = values[4]*256+values[5];
}

void retrive(ftp ftp, url url){
  char cmd[MAX_STRING_SIZE];

  sprintf(cmd, "RETR %s%s\r\n", url.file_path, url.filename);
  if(socketWrite(ftp.ctrl_socket_fd, cmd) != 0){
    fprintf(stderr, "Error retrieving file. Exiting...\n");
    exit(1);
  }

  socketRead(ftp.ctrl_socket_fd,NULL);
}

int download(ftp ftp, url url){

  FILE* dest_file;
  if(!(dest_file = fopen(url.filename, "w"))) {
		printf("ERROR: Cannot open file.\n");
		return 1;
	}

  char buf[1024];
  int i;
  while ((i = read(ftp.data_socket_fd, buf, sizeof(buf)))) {
    if (i < 0) {
      fprintf(stderr, "ERROR: Nothing was received from data socket fd.\n");
      return 1;
    }

    if ((i = fwrite(buf, i, 1, dest_file)) < 0) {
      fprintf(stderr, "ERROR: Cannot write data in file.\n");
      return 1;
    }
  }

  fclose(dest_file);

  printf("Finished downloading file\n");

  return 0;
}


int endConnection(ftp ftp){

  printf("Closing connection\n");
  if(socketWrite(ftp.ctrl_socket_fd, "QUIT\r\n") != 0){
    fprintf(stderr, "Error closing connection. Exiting anyway...\n");
  }

  close(ftp.data_socket_fd);
  close(ftp.ctrl_socket_fd);

  printf("Goodbye!\n");

  return 0;
}
