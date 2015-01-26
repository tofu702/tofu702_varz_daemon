#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


#include <netinet/ip.h>
#include <sys/socket.h>

#include "executor.h"
#include "input_parser.h"


#define RECV_BUFSIZE 1638
#define UDP_PORT  4447

#define EXECUTOR_HT_SIZE 1024


/***** STATIC METHOD PROTOTYPES *****/
static int setupUDPSocketAndReturnFD();
static void handleUDPPacket(int udp_fd, VARZExecutor_t *executor);


/***** IMPLEMENTATION *****/


int main(int argc, char **argv) {
  VARZExecutor_t executor;
  int udp_fd = setupUDPSocketAndReturnFD();


  VARZExecutorInit(&executor, EXECUTOR_HT_SIZE);
  printf("fd=%d\n", udp_fd);

  while (1) {
    handleUDPPacket(udp_fd, &executor);
  }

  VARZExecutorFree(&executor);
  return 0;
}


static int setupUDPSocketAndReturnFD() {
  int fd; 
  struct sockaddr_in myaddr;

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "ERROR CANNOT OPEN SOCKET\n");
    exit(1);
  }

  bzero(&myaddr, sizeof(struct sockaddr_in));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(UDP_PORT);

  if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    fprintf(stderr, "ERROR, CANNOT BIND\n");
    exit(1);
  }
  return fd;
}


static void handleUDPPacket(int udp_fd, VARZExecutor_t *executor) {
  uint8_t recv_buf[RECV_BUFSIZE];
  int len;
  struct sockaddr_in src_addr;
  struct VARZOperationDescription desc;
  socklen_t src_addr_len;

  src_addr_len = sizeof(struct sockaddr_in);
  //Do the -1 to allow for the NULL terminator
  len = recvfrom(udp_fd, recv_buf, RECV_BUFSIZE-1, 0, (struct sockaddr *)&src_addr, &src_addr_len);
  recv_buf[len] = '\0';
  desc = VARZOpCmdParse((char*)recv_buf);

  printf("desc.op=%d\n", (int)desc.op);

  VARZExecutorExecute(executor, &desc);
}
