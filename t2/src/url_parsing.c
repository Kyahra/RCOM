#include "url_parsing.h"



int userPassword(url * info_struct, char * complete_url,char * at){
  char* first_slash = strchr(url, '/'); //slash is never null
 first_slash += 2; // slash * its increased by two to point to the beggining of the user name "//[username]"
 char* password = strchr(first_slash, ':');
 if(password == NULL){
    fprintf(stderr, "Your link must contain a ':' separating the username and password!'\n");
    return 1;
  }
   memcpy(info->user, slash, password - slash);

}

int parse_url(char complete_url[],url * info_struct){
  char * ftp;
  strcpy(ftp,"ftp://");
  if(strncmp(complete_url, ftp, strlen(ftp)) != 0){
     fprintf(stderr, "The link does not begin with 'ftp://'\n");
     return 1;
   }

   char * at = strrchr(complete_url, '@');

   if(at==NULL){
     memcpy(info->user, "anonymous", strlen("anonymous") + 1);
     memcpy(info->password, "mail@domain", strlen("mail@domain") + 1);
   }
   else{

   }


}
