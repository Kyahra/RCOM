#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "TCP.h"

#define READ 1
#define NO_READ 0


int initConnection(ftp* ftp,char* ip_address, int port){
  int socketfd;

  if ((socketfd = initSocket(ip_address, port)) < 0) {
		printf("ERROR: Cannot connect socket.\n");
		return 1;
	}

	ftp->ctrl_socket_fd = socketfd;
  ftp->data_socket_fd = 0;

  return 0;

}

int initSocket(char* ip_address, int port){

  int	socketfd;
  struct	sockaddr_in server_addr;

  /*server address handling*/
  bzero((char*)&server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip_address);	/*32 bit Internet address network byte ordered*/
  server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */

  /*open an TCP socket*/
  if ((socketfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
        perror("socket()");
          return -1;
      }
  /*connect to the server*/
      if(connect(socketfd,
             (struct sockaddr *)&server_addr,
       sizeof(server_addr)) < 0){
          perror("connect()");
    return -1;
  }
  return socketfd;

}

void login(ftp ftp, url url){
  char usr_cmd[MAX_SIZE];
  char pwd_cmd[MAX_SIZE];

  socketRead(ftp.ctrl_socket_fd, NULL);

  sprintf(usr_cmd, "USER %s\r\n", url.user);
  printf(">%s",usr_cmd);

  socketWrite(ftp.ctrl_socket_fd,usr_cmd);
  socketRead(ftp.ctrl_socket_fd,NULL);

  sprintf(pwd_cmd, "PASS %s\r\n", url.password);
  printf(">%s",pwd_cmd);

  socketWrite(ftp.ctrl_socket_fd, pwd_cmd);
  if(socketRead(ftp.ctrl_socket_fd,NULL) !=0){
    fprintf(stderr, "Wrong credentials. Exiting...\n");
    exit(1);
  }
}

void passiveMode(ftp ftp, char* ip_adress, int* port){
  char repply[MAX_SIZE];

  socketWrite(ftp.ctrl_socket_fd, "PASV\r\n");
  if(socketRead(ftp.ctrl_socket_fd,repply) !=0){
    fprintf(stderr, "Error entering passive mode. Exiting...\n");
    exit(1);
  }

  int values[6];
  char* data = strchr(repply, '(');
  sscanf(data, "(%d, %d, %d, %d, %d, %d)", &values[0],&values[1],&values[2],&values[3],&values[4],&values[5]);
  sprintf(ip_adress, "%d.%d.%d.%d", values[0],values[1],values[2],values[3]);
  *port = values[4]*256+values[5];
}

void retrieve(ftp ftp, url url){
  char cmd[MAX_SIZE];

  sprintf(cmd, "RETR %s%s\r\n", url.file_path, url.file_name);
  printf(">%s",cmd);
  socketWrite(ftp.ctrl_socket_fd, cmd);

  if(socketRead(ftp.ctrl_socket_fd,NULL) != 0){
    fprintf(stderr, "Error retrieving file. Exiting...\n");
    exit(1);
  }
}

int socketRead(int socketfd, char* repply){
  FILE* fp = fdopen(socketfd, "r");
  int allocated = 0;

  if(repply == NULL){
    repply = (char*) malloc(sizeof(char) * MAX_SIZE);
    allocated = 1;
  }

  do {
    memset(repply, 0, MAX_SIZE);
    repply = fgets(repply, MAX_SIZE, fp);
    printf("<%s", repply);
  } while (!('1' <= repply[0] && repply[0] <= '5') || repply[3] != ' ');

  char r0= repply[0];

  if(allocated)
    free(repply);

  return (r0>'4');
}

int socketWrite(int socketfd, char* cmd){

    int ret = write(socketfd, cmd, strlen(cmd));
    return ret;
}

int download(ftp ftp, url url){
  FILE* dest_file;
  if(!(dest_file = fopen(url.file_name, "w"))) {
		printf("Error opening file %s.\n",url.file_name);
		return 1;
	}

  char buf[1024];
  int bytes;
  while ((bytes = read(ftp.data_socket_fd, buf, sizeof(buf)))) {
    if (bytes < 0) {
      fprintf(stderr, "Error, nothing was received from data socket fd.\n");
      return 1;
    }

    if ((bytes = fwrite(buf, bytes, 1, dest_file)) < 0) {
      fprintf(stderr, "Error, cannot write data in file.\n");
      return 1;
    }
  }

  fclose(dest_file);

  printf("Finished downloading file\n");

  return 0;
}


int endConnection(ftp ftp){

  printf("Closing connection\n");
  socketWrite(ftp.ctrl_socket_fd,"QUIT\r\n");

  if(socketRead(ftp.ctrl_socket_fd,NULL) != 0){
    fprintf(stderr, "Error closing connection. Closing...\n");
    close(ftp.data_socket_fd);
    close(ftp.ctrl_socket_fd);
    exit(1);
  }

  close(ftp.data_socket_fd);
  close(ftp.ctrl_socket_fd);

  printf("Ending conection! Come back soon\n");

  return 0;
}
