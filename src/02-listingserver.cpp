#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <ctype.h>



#define SIZE 1024
char buf[SIZE];
char *cmd[5];

#define PORT 9005

int parseline(char * line ) {
char * p;
int count = 0;
p = strtok(line, " ");
while (p && strcmp(p,"|") != 0) {
cmd[count] = p;
count++;
p = strtok(NULL," ");
}
cmd[count]=NULL;
return count;
}
int main(int argc,
         char *argv[])
{
  int sockfd, client_sockfd;
  int nread;
  socklen_t len;
  struct sockaddr_in serv_addr,
                   client_addr;
  int i,j;
  char ch;
  uint16_t serv_port=PORT;
  pid_t ForkPID;
  if(argc!=2)
  {
	  fprintf(stderr,"\nUsage: /.listingserver <PORT> \n\n");
	     exit(1);
  }
  serv_port=atoi(argv[1]);

  /* create endpoint */
  if ((sockfd = socket(AF_INET,SOCK_STREAM, 0))< 0)
  {
    perror(NULL);
    exit(2);
  }

  /* bind address */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(serv_port);

  if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror(NULL);
    exit(3);
  }

  /* specify queue */
  listen(sockfd, 5);

    for (;;) {
  	  printf("\nListening socket for Incoming Connections ..\n");
      len = sizeof(client_addr);
      client_sockfd = accept(sockfd,(struct sockaddr*)&client_addr,&len);
      printf("\nAccepting Connection Successful..\n");
      if (client_sockfd == -1) {
        perror(NULL);
        continue;
      }

      printf("\nListingServer waiting to read from new Connection...\n");


//Reads the argument from listingclient and sends the output back to the listingclient
      //Must exit program using ^c
    nread=read(client_sockfd,buf,SIZE);
    buf[nread]='\0';



printf("Request :%s\n",buf);

    ForkPID = fork();
    switch (ForkPID) {
    	case -1: printf("Error: Failed to fork.\n"); break;
    	case 0:
    		parseline(buf);
    		dup2(client_sockfd,STDOUT_FILENO);
    		if (execvp(cmd[0],cmd) == -1) {
    		printf("Error: running command: '%s'\n",cmd[0]);
    		exit(0);
    		}
    		exit(0);
    	break;
    	default:
        waitpid(ForkPID,NULL,0);
    }
    close(client_sockfd);
  }
}
