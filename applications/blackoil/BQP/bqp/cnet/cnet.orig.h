#ifndef INCLUDE_CNET_H
#define INCLUDE_CNET_H

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

int cnet_init(int port);
int cnet_connect(char *hostname, int port, int *sfd);
int cnet_accept(int );
int cnet_close(int );

int cnet_send(int sfd, const void *buf, int bytes);
int cnet_recv(int sfd, void *buf, int bytes);

int cnet_sendbuf(int sfd, const void *buf, int bytes);
int cnet_recvbuf(int sfd, void *buf, int bytes);

int cnet_protsend(int sfd, const void *buf, int bytes);
int cnet_protrecv(int sfd, void **buf);

#endif
