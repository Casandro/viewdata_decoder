#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "viewdata_network.h"
#include "viewdata_screen.h"

#define SA struct sockaddr

static int sockfd=-1;

int viewdata_connect_socket(const char *ip, const int port)
{
	int sockfd=socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd==-1) return sockfd;
	fprintf(stderr, "viewdata connect %s %d\n", ip, port);
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr= inet_addr(ip);
	servaddr.sin_port=htons(port);
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!=0) {
		fprintf(stderr, "Couldn't connect to %s:%d\n", ip, port);
		return -1;
	}
	struct timeval tv;
	tv.tv_sec=0;
	tv.tv_usec=40*1000;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	return sockfd;
}


int viewdata_handle_socket(const int sockfd)
{
	int sum=0;
	while (0==0) {
		uint8_t buf[2048];
		ssize_t rs=read(sockfd, buf, sizeof(buf));
		if ((rs<0) && (errno==EWOULDBLOCK)) return 0; //No data to process
		if (rs<=0) return rs; //Otherwise an error
		viewdata_handle_string(buf, rs);
		sum=sum+rs;
	}
	return sum;
}

int viewdata_send_to_socket(const int sockfd, const char c)
{
	char b=c;
	ssize_t res=write(sockfd, &b, 1);
	if (res!=1) {
		fprintf(stderr,"viewdata_send_to_socket, error: %d %d\n", errno, res);
	}
	return 0;
}

int viewdata_connect(const char *ip, const int port)
{
	sockfd=viewdata_connect_socket(ip, port);
	if (sockfd<0) return -1;
	return viewdata_handle_socket(sockfd);
}

int viewdata_handle(int input)
{
	if (sockfd<0) return -1;
	if ((input>=0) && (input<256)) viewdata_send_to_socket(sockfd, input);
	return viewdata_handle_socket(sockfd);
}
