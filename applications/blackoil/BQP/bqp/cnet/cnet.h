#ifndef INCLUDE_CNET_H
#define INCLUDE_CNET_H
#ifdef __cplusplus                                                      
extern "C" {                                                            
#endif       
 
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//int cnet_sfd;
  //struct sockaddr_in cnet_saddr, cnet_caddr;

int cnet_init(int port);
int cnet_connect(char *hostname, int port, int *sfd, int tosec);
int cnet_accept(int, int tosec);
int cnet_close(int cnet_sfd);

int cnet_send(int sfd, const void *buf, int bytes, int tosec);
int cnet_recv(int sfd, void *buf, int bytes, int tosec);

int cnet_sendbuf(int sfd, const void *buf, int bytes, int tosec);
int cnet_recvbuf(int sfd, void *buf, int bytes, int tosec);

int cnet_protsend(int sfd, const void *buf, int bytes, int tosec);
int cnet_protrecv(int sfd, void **buf, int tosec);

struct hostent *mygethostbyname (char *);

#ifdef __cplusplus                                                      
}         
#endif
#endif
