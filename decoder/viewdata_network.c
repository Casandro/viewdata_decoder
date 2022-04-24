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
#include <string.h>

#include "viewdata_network.h"
#include "viewdata_screen.h"

#define SA struct sockaddr

static int sockfd=-1;

int viewdata_connect_socket(const char *hostname, const int port)
{
	struct hostent *h=gethostbyname(hostname);
	if (h==NULL) {
		fprintf(stderr, "Couldn't resolve hostname %s\n", hostname);
		return -1;
	}
	int sockfd=socket(h->h_addrtype, SOCK_STREAM, 0);
	if (sockfd==-1) return sockfd;
	fprintf(stderr, "viewdata connect %s %d\n", hostname, port);
	if (h->h_addrtype==AF_INET) {
		struct sockaddr_in servaddr;
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family=AF_INET;
		memcpy(&(servaddr.sin_addr.s_addr), h->h_addr, 4);
		servaddr.sin_port=htons(port);
		if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!=0) {
			fprintf(stderr, "Couldn't connect to %s:%d\n", hostname, port);
			return -1;
		}
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
		fprintf(stderr,"viewdata_send_to_socket, error: %d %ld\n", errno, res);
	}
	return 0;
}

int viewdata_connect(const char *hostname, const int port)
{
	sockfd=viewdata_connect_socket(hostname, port);
	if (sockfd<0) return -1;
	return viewdata_handle_socket(sockfd);
}

int viewdata_handle(int input)
{
	if (sockfd<0) return -1;
	if ((input>=0) && (input<256)) viewdata_send_to_socket(sockfd, input);
	return viewdata_handle_socket(sockfd);
}
