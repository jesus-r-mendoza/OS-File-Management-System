
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>


#define SIZE 1024
char buf[SIZE];

#define DPORT 9005


int main(int argc, char *argv[])
{
  int sockfd, client_sockfd;
  int nread;
  socklen_t len;
  struct sockaddr_in serv_addr,
                   client_addr;
  int i,j;
  char ch;
  uint16_t serv_port = DPORT;

  if(argc!=2)
  {
	  fprintf(stderr,"\nUsage: /server <PORT> \n\n");
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

     printf("\nBasic server waiting to read from new Connection...\n");


    /*
     * Here the value is read from the socket, the string is reversed and written back to the client socket.
     * The server keeps running till exited as ^c
     */
    nread=read(client_sockfd,buf,SIZE);
    buf[nread]='\0';
    printf("Request : %s\n",buf);
    for(i=0,j=strlen(buf)-1;i<j;i++,j--)
    				{
    	       	   	    ch = buf[i];
    					buf[i]=buf[j];
    					buf[j]=ch;
    				}
    len = strlen(buf) + 1;
    printf("Response :%s\n",buf);
    write(client_sockfd, buf, len);
    close(client_sockfd);
  }
}
