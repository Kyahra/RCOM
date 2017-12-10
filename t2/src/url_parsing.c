#include "url_parsing.h"



int userPassword(url * info_struct, char * complete_url){
  char * at = strrchr(complete_url, '@');
  char* first_slash = strchr(complete_url, '/'); //slash is never null
  first_slash += 2; // first_slash * its increased by two to point to the beggining of the user name "//[username]"
  char* password = strchr(first_slash, ':');
  if(password == NULL){
    fprintf(stderr, "Your link must contain a ':' separating the username and password!'\n");
    return 1;
  }
  memcpy(info_struct->user, first_slash, password - first_slash); //password - slash it's the size of username in bytes
  info_struct->user[password-first_slash]=0;
  password++; //the password pointer was poiting to ":" and it has to point to the first character of the userPassword
  memcpy(info_struct->password,password,at - password);
  info_struct->password[at-password] = 0; //string end character
  return 0;
}

int parse_url(char complete_url[],url * info_struct){
  if(strncmp(complete_url, "ftp://", strlen("ftp://")) != 0){
    fprintf(stderr, "The link does not begin with 'ftp://'\n");
    return 1;
}

  char* slash_after_host;

  if(!strchr(complete_url, '@')){
    memcpy(info_struct->user, "anonymous", strlen("anonymous") + 1);
    memcpy(info_struct->password, "anonymous", strlen("anonymous") + 1);

    char * s1 = strchr(complete_url,'/');
    s1++;
    s1++;

    slash_after_host = strchr(s1, '/');
    memcpy(info_struct->host, s1, slash_after_host-s1);
    info_struct->host[slash_after_host-s1] = 0;
  }
  else{
    if(userPassword(info_struct,complete_url)!=0)
      return 1;

      char * at = strrchr(complete_url, '@');
      at++;

      slash_after_host = strchr(at, '/');
      memcpy(info_struct->host, at, slash_after_host-at);
      info_struct->host[slash_after_host-at] = 0;

  }


  char* last_slash = strrchr(complete_url, '/');
  last_slash++; //to point to the element after the slash
  memcpy(info_struct->file_path, slash_after_host, last_slash-slash_after_host);
  info_struct->file_path[last_slash-slash_after_host] = 0;

  memcpy(info_struct->file_name, last_slash, strlen(last_slash) + 1);

  getIp(info_struct);

  return 0;
}


int getIp(url* info_struct) {
	struct hostent* h;

	if ((h = gethostbyname(info_struct->host)) == NULL) {
		herror("gethostbyname");
		return 1;
	}


	char* ip = inet_ntoa(*((struct in_addr *) h->h_addr));
	strcpy(info_struct->ip, ip);

	return 0;
}
