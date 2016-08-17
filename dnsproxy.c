#include"dnsproxy.h"

PROXY_ENGINE g_engine;

void process_query(PROXY_ENGINE *engine)
{
  LOCAL_DNS *ldns;
  REMOTE_DNS *rdns;
  DNS_HDR *hdr, *rhdr;
  DNS_QDS *qds;
  // DNS_RRS *rrs;
  socklen_t addrlen;
  struct sockaddr_in source;
  char *pos, *head, *rear;
  char *buffer, domain[PACKAGE_SIZE], rbuffer[PACKAGE_SIZE];
  int size, dlen;
  // time_t current;
  unsigned char qlen;
  //unsigned int ttl, ttl_tmp;
  unsigned short  q_len;// index;

  ldns = &engine->local;
  rdns = &engine->remote;
  buffer = ldns->buffer + sizeof(unsigned short);

  addrlen = sizeof(struct sockaddr_in);
  size = recvfrom(ldns->sock, buffer, PACKAGE_SIZE, 0, (struct sockaddr*)&source, &addrlen);
  if(size <= (int)sizeof(DNS_HDR))
    return;

  hdr = (DNS_HDR*)buffer;
  rhdr = (DNS_HDR*)rbuffer;
  memset(rbuffer, 0, sizeof(DNS_HDR));

  rhdr->id = hdr->id;
  rhdr->qr = 1;
  q_len = 0;
  qds = NULL;
  head = buffer + sizeof(DNS_HDR);
  rear = buffer + size;
  if(hdr->qr != 0 || hdr->tc != 0 || ntohs(hdr->qd_count) != 1)
    rhdr->rcode = 1;
  else
    {
      dlen = 0;
      pos = head;
      while(pos < rear)
        {
          qlen = (unsigned char)*pos++;
          if(qlen > 63 || (pos + qlen) > (rear - sizeof(DNS_QDS)))
            {
              rhdr->rcode = 1;
              break;
            }
          if(qlen > 0)
            {
              if(dlen > 0)
                domain[dlen++] = '.';
              while(qlen-- > 0)
                domain[dlen++] = (char)tolower(*pos++);
            }
          else
            {
              qds = (DNS_QDS*) pos;
              if(ntohs(qds->classes) != 0x01)
                rhdr->rcode = 4;
              else
                {
                  pos += sizeof(DNS_QDS);
                  q_len = pos - head;
                }
              break;
            }
        }
      domain[dlen] = '\0';
    }

  if(rhdr->rcode == 0)
    {

      if(rdns->sock == INVALID_SOCKET)
        {
          rdns->head = 0;
          rdns->rear = 0;
          rdns->sock = socket(AF_INET, SOCK_STREAM, 0);
          if(rdns->sock != INVALID_SOCKET)
            {
              setsockopt(rdns->sock, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
              if(connect(rdns->sock, (struct sockaddr*)&rdns->addr, sizeof(struct sockaddr_in)) != 0)
                {
                  closesocket(rdns->sock);
                  rdns->sock = INVALID_SOCKET;
                }
            }
        }
      if(rdns->sock == INVALID_SOCKET)
        rhdr->rcode = 2;
      else
        {
          pos = ldns->buffer;
          *(unsigned short*)pos = htons((unsigned short)size);
          size += sizeof(unsigned short);
          if(send(rdns->sock, ldns->buffer, size, 0) != size)
            {
              rdns->head = 0;
              rdns->rear = 0;
              closesocket(rdns->sock);
              rdns->sock = INVALID_SOCKET;
              rhdr->rcode = 2;
            }
        }
    }

  if(rhdr->rcode != 0)
    {
      sendto(ldns->sock, rbuffer, sizeof(DNS_HDR), 0, (struct sockaddr*)&source, sizeof(struct sockaddr_in));
    }
}


void process_response(char* buffer, int size)
{
  DNS_HDR *hdr;
  DNS_QDS *qds;
  DNS_RRS *rrs;
  //LOCAL_DNS *ldns;
  char domain[PACKAGE_SIZE];
  char *pos, *rear, *answer;
  int badfmt, dlen;
  int length;
  unsigned char qlen;
  unsigned int ttl, ttl_tmp;
  unsigned short index, an_count;


  length = size;
  hdr = (DNS_HDR*)buffer;
  an_count = ntohs(hdr->an_count);
  if(hdr->qr == 1 && hdr->tc == 0 && ntohs(hdr->qd_count) == 1 && an_count > 0)
    {
      dlen = 0;
      qds = NULL;
      pos = buffer + sizeof(DNS_HDR);
      rear = buffer + size;
      while(pos < rear)
        {
          qlen = (unsigned char)*pos++;
          if(qlen > 63 || (pos + qlen) > (rear - sizeof(DNS_QDS)))
            break;
          if(qlen > 0) {
              if(dlen > 0)
                domain[dlen++] = '.';
              while(qlen-- > 0)
                domain[dlen++] = (char)tolower(*pos++);
            }
          else
            {
              qds = (DNS_QDS*) pos;
              if(ntohs(qds->classes) != 0x01)
                qds = NULL;
              else
                pos += sizeof(DNS_QDS);
              break;
            }
        }
      domain[dlen] = '\0';

      if(qds && ntohs(qds->type) == 0x01)
        {
          index = 0;
          badfmt = 0;
          answer = pos;
          while(badfmt == 0 && pos < rear && index++ < an_count)
            {
              rrs = NULL;
              if((unsigned char)*pos == 0xc0)
                {
                  pos += 2;
                  rrs = (DNS_RRS*) pos;
                }
              else
                {
                  while(pos < rear)
                    {
                      qlen = (unsigned char)*pos++;
                      if(qlen > 63 || (pos + qlen) > (rear - sizeof(DNS_RRS)))
                        break;
                      if(qlen > 0)
                        pos += qlen;
                      else
                        {
                          rrs = (DNS_RRS*) pos;
                          break;
                        }
                    }
                }
              if(rrs == NULL || ntohs(rrs->classes) != 0x01)
                badfmt = 1;
              else
                {
                  ttl_tmp = ntohl(rrs->ttl);
                  if(ttl_tmp < ttl)
                    ttl = ttl_tmp;
                  pos += sizeof(DNS_RRS) + ntohs(rrs->rd_length);
                }
            }
          if(badfmt == 0)
            {
              hdr->nr_count = 0;
              hdr->ns_count = 0;
              length = pos - buffer;

            }
        }
    }

}

void process_response_tcp(REMOTE_DNS *rdns)
{
  char *pos;
  int to_down, size;
  unsigned int len, buflen;


  to_down = 0;
  size = recv(rdns->sock, rdns->buffer + rdns->rear, rdns->capacity - rdns->rear, 0);
  if(size < 1)
    to_down = 1;
  else
    {
      rdns->rear += size;
      while((buflen = rdns->rear - rdns->head) > sizeof(unsigned short))
        {
          pos = rdns->buffer + rdns->head;
          len = ntohs(*(unsigned short*)pos);
          if(len > PACKAGE_SIZE)
            {
              to_down = 1;
              break;
            }
          if(len + sizeof(unsigned short) > buflen)
            break;
          process_response(pos + sizeof(unsigned short), len);
          rdns->head += len + sizeof(unsigned short);
        }

      if(!to_down)
        {
          if(rdns->head == rdns->rear)
            {
              rdns->head = 0;
              rdns->rear = 0;
            }
          else if(rdns->head > PACKAGE_SIZE)
            {
              len = rdns->rear - rdns->head;
              memmove(rdns->buffer, rdns->buffer + rdns->head, len);
              rdns->head = 0;
              rdns->rear = len;
            }
        }
    }

  if(to_down)
    {
      rdns->head = 0;
      rdns->rear = 0;
      closesocket(rdns->sock);
      rdns->sock = INVALID_SOCKET;
    }
}



int dnsproxy(const char* remote_addr)
{
  unsigned short local_port=5353;
  unsigned short remote_port=53;
  int maxfd, fds;
  fd_set readfds;
  struct timeval timeout;
  struct sockaddr_in addr;
  PROXY_ENGINE *engine = &g_engine;
  LOCAL_DNS *ldns = &engine->local;
  REMOTE_DNS *rdns = &engine->remote;

  ldns->sock = socket(AF_INET, SOCK_DGRAM, 0);

  if(ldns->sock == INVALID_SOCKET)
    {
      perror("create socket");
      return -1;
    }
  setsockopt(ldns->sock, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(local_port);
  if(bind(ldns->sock, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
      perror("bind service port");
      return -1;
    }


  rdns->sock = INVALID_SOCKET;
  rdns->addr.sin_family = AF_INET;
  rdns->addr.sin_addr.s_addr = inet_addr(remote_addr);
  rdns->addr.sin_port = htons(remote_port);
  rdns->head = 0;
  rdns->rear = 0;
  rdns->capacity = sizeof(rdns->buffer);


  while(1)
    {
      FD_ZERO(&readfds);
      FD_SET(ldns->sock, &readfds);
      maxfd = (int)ldns->sock;
      if(rdns->sock != INVALID_SOCKET)
        {
          FD_SET(rdns->sock, &readfds);
          if(maxfd < (int)rdns->sock)
            maxfd = (int)rdns->sock;
        }
      timeout.tv_usec = 0;
      timeout.tv_sec = 0;
      fds = select(maxfd + 1, &readfds, NULL, NULL, &timeout);


      if(fds > 0)
        {
          if(rdns->sock != INVALID_SOCKET && FD_ISSET(rdns->sock, &readfds))
            {
              // fprintf(stderr,"process_response_tcp\n");
              process_response_tcp(rdns);

            }
          if(FD_ISSET(ldns->sock, &readfds))
            {
              // fprintf(stderr,"process_query\n");
              process_query(engine);
            }
        }

    }
  return 0;
}


