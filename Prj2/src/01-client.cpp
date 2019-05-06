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



#define SIZE 1024
char buf[SIZE];
#define DPORT 9005

int main(int argc,
         char *argv[])
{
  int sockfd;
  int nread;
  struct sockaddr_in serv_addr;
  struct hostent *host;
  uint16_t serv_port=DPORT;
  char *ip;

  if (argc != 4)
  {
    fprintf(stderr,"\nUsage: /client <IP> <PORT> <STRING>\n\n");
    exit(1);
  }
  ip=argv[1];
  serv_port=atoi(argv[2]);

  /* create endpoint */
  if ((sockfd = socket(AF_INET,SOCK_STREAM, 0))< 0) {
    perror(NULL);
    exit(2);
  }

  /* connect to server */
  serv_addr.sin_family = AF_INET;
  host = gethostbyname(argv[1]);
  memcpy(&serv_addr.sin_addr.s_addr, host->h_addr, host->h_length);
  serv_addr.sin_port =
            htons(serv_port);
  printf("\nSending Connection request to Basic server...\n");
  if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr))< 0) {
    perror(NULL);
    exit(3);
  }
  printf("\nSuccessfully Connected to Basic server..\n");

  /* transfer data */
  char *str;
  str=argv[3];
  printf("Request : %s\n\n",str);
  nread=write(sockfd,str,strlen(str)); //Send the string
  nread = read(sockfd, buf,SIZE); //Retrieves the string
  write(1, buf, nread);// Prints the output
  printf("\n\n");
  close(sockfd);
  exit(0);
}