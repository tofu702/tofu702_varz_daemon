#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


#include <netinet/ip.h>
#include <sys/socket.h>


#define RECV_BUFSIZE 1638

int main(int argc, char **argv) {
  int fd; 
  struct sockaddr_in myaddr;
  uint8_t recv_buf[RECV_BUFSIZE];

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "ERROR CANNOT OPEN SOCKET\n");
    return 1;
  }
 
  bzero(&myaddr, sizeof(struct sockaddr_in));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(32000);

  if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    fprintf(stderr, "ERROR, CANNOT BIND\n");
    return 1;
  }

  printf("fd=%d\n", fd);

  while (1) {
    struct sockaddr_in src_addr;
    socklen_t src_addr_len = sizeof(struct sockaddr_in);
    int len = recvfrom(fd, recv_buf, RECV_BUFSIZE, 0, &src_addr, &src_addr_len);
    fprintf(stderr, "****Got MSG of len: %d\n", len);
    recv_buf[len] = '\0';
    fprintf(stderr, "%s\n", recv_buf);
    return 0;
  }

  return 0;
}

