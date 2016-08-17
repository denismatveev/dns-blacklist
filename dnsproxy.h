#ifndef DNSPROXY_H
#define DNSPROXY_H
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define closesocket close
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PACKAGE_SIZE 8192

typedef struct {
  unsigned short id;       // identification number
  unsigned char rd :1;     // recursion desired
  unsigned char tc :1;     // truncated message
  unsigned char aa :1;     // authoritive answer
  unsigned char opcode :4; // purpose of message
  unsigned char qr :1;     // query/response flag
  unsigned char rcode :4;  // response code
  unsigned char cd :1;     // checking disabled
  unsigned char ad :1;     // authenticated data
  unsigned char z :1;      // its z! reserved
  unsigned char ra :1;     // recursion available
  unsigned short qd_count; // number of question entries
  unsigned short an_count; // number of answer entries
  unsigned short ns_count; // number of authority entries
  unsigned short nr_count; // number of resource entries
} DNS_HDR;

typedef struct {
  unsigned short type;
  unsigned short classes;
} DNS_QDS;

typedef struct {
  unsigned short type;
  unsigned short classes;
  unsigned int ttl;
  unsigned short rd_length;
  char rd_data[0];
} DNS_RRS;

typedef struct {
  SOCKET sock;
  char buffer[PACKAGE_SIZE + sizeof(unsigned short)];
} LOCAL_DNS;//use udp

typedef struct {
  SOCKET sock;
  struct sockaddr_in addr;
  unsigned int head;
  unsigned int rear;
  unsigned int capacity;
  char buffer[PACKAGE_SIZE * 3];
} REMOTE_DNS;//use tcp

typedef struct {
  LOCAL_DNS local;
  REMOTE_DNS remote;
} PROXY_ENGINE;

static const int enable = 1;
void process_query(PROXY_ENGINE*);
void process_response(char*, int);
void process_response_tcp(REMOTE_DNS*);
int dnsproxy(const char*);

#endif // DNSPROXY_HPP
