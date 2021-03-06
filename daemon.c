#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <netinet/ip.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "executor.h"
#include "input_parser.h"
#include "numeric_utils.h"


#define RECV_BUFSIZE 1638
#define UDP_PORT  4447
#define TCP_PORT 14447
#define TCP_LISTEN_QUEUE_SIZE 10

#define EXECUTOR_HT_SIZE 1024
#define DESIRED_UDP_RCV_BUF_SIZE (64*1024*1024) //Why not, let's aim for the stars


/***** STATIC METHOD PROTOTYPES *****/
static int setupUDPSocketAndReturnFD();
static int setupTCPSocketAndReturnFD();
static void handleUDPPacket(int udp_fd, VARZExecutor_t *executor);
static void handleNewTCPConnection(int tcp_fd, VARZExecutor_t *executor);
static void sendall(int tcp_fd, void *buf, size_t length, int flags);


/***** IMPLEMENTATION *****/


int main(int argc, char **argv) {
  VARZExecutor_t executor;
  int udp_fd, tcp_fd;
  fd_set readfds;
  udp_fd = setupUDPSocketAndReturnFD();
  tcp_fd = setupTCPSocketAndReturnFD();


  VARZExecutorInit(&executor, EXECUTOR_HT_SIZE);

  while (1) {
    FD_ZERO(&readfds);
    FD_SET(udp_fd, &readfds);
    FD_SET(tcp_fd, &readfds);
    select(MAX(udp_fd, tcp_fd)+1, &readfds, NULL, NULL, NULL);

    if (FD_ISSET(udp_fd, &readfds)) {
      handleUDPPacket(udp_fd, &executor);
    } else if (FD_ISSET(tcp_fd, &readfds)) {
      handleNewTCPConnection(tcp_fd, &executor);
    }
  }

  VARZExecutorFree(&executor);
  return 0;
}


static int setupUDPSocketAndReturnFD() {
  int fd; 
  struct sockaddr_in myaddr;
  int reuse_optval;
  unsigned int actual_rcv_buf_bytes_size;
  size_t desired_rcv_buf_size, actual_rcv_buf_bytes=0;

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "ERROR CANNOT OPEN UDP SOCKET\n");
    exit(1);
  }

  reuse_optval = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_optval, sizeof(reuse_optval)) != 0) {
    fprintf(stderr, "ERROR: Unable to set UDP socket to reuse\n");
  }

  desired_rcv_buf_size = DESIRED_UDP_RCV_BUF_SIZE;
  actual_rcv_buf_bytes_size = sizeof(actual_rcv_buf_bytes);
  setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &desired_rcv_buf_size, sizeof(desired_rcv_buf_size));
  getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &actual_rcv_buf_bytes, &actual_rcv_buf_bytes_size);
  printf("Setting up UDP socket RCVBUF size = %lu bytes\n", actual_rcv_buf_bytes);

  memset(&myaddr, 0, sizeof(struct sockaddr_in));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(UDP_PORT);

  if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    fprintf(stderr, "ERROR, CANNOT BIND UDP\n");
    exit(1);
  }
  return fd;
}


static int setupTCPSocketAndReturnFD() {
  int fd;
  struct sockaddr_in myaddr;
  int reuse_optval;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "ERROR: CANNOT OPEN TCP SOCKET\n");
    exit(1);
  }

  reuse_optval = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_optval, sizeof(reuse_optval)) != 0) {
    fprintf(stderr, "ERROR: Unable to set UDP socket to reuse\n");
  }

  memset(&myaddr, 0, sizeof(struct sockaddr_in));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(TCP_PORT);

  if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    fprintf(stderr, "ERROR: Cannot Bind TCP\n");
    exit(1);
  }
  listen(fd, TCP_LISTEN_QUEUE_SIZE);
  return fd;
}

static void handleUDPPacket(int udp_fd, VARZExecutor_t *executor) {
  uint8_t recv_buf[RECV_BUFSIZE];
  int len;
  struct VARZOperationDescription desc;

  //Do the -1 to allow for the NULL terminator
  len = recv(udp_fd, recv_buf, RECV_BUFSIZE-1, 0);
  recv_buf[len] = '\0';
  desc = VARZOpCmdParse((char*)recv_buf, len);

  void *result = VARZExecutorExecute(executor, &desc);
  //TODO: REMOVE & Return
  if (result) {
    free(result);
  }
}

static void handleNewTCPConnection(int tcp_fd, VARZExecutor_t *executor) {
  struct sockaddr_storage incoming_addr;
  uint8_t recv_buf[RECV_BUFSIZE];
  int conn_fd, recved_bytes, send_flags=0;
  socklen_t recv_buf_len;
  struct VARZOperationDescription desc;

  recv_buf_len  = sizeof(recv_buf) - 1;
  if ((conn_fd = accept(tcp_fd, (struct sockaddr *) &incoming_addr, &recv_buf_len)) < 0) {
    fprintf(stderr, "Unable to get FD for TCP accept()\n");
    exit(1);
  }

  // SIGPIPES crash the whole program..., we need to handle them differently on linux than on
  // OS X
  #ifdef SO_NOSIGPIPE // OS X
    int disable_sigpipe = 1;
    setsockopt(conn_fd, SOL_SOCKET, SO_NOSIGPIPE, &disable_sigpipe, sizeof(disable_sigpipe));
  #else // LINUX
    send_flags |= MSG_NOSIGNAL;
  #endif

  // TODO: We might want a recv_all in the future...
  recved_bytes = recv(conn_fd, recv_buf, sizeof(recv_buf)-1, 0);
  recv_buf[recved_bytes] = '\0';

  desc = VARZOpCmdParse((char*)recv_buf, recved_bytes);

  void *result = VARZExecutorExecute(executor, &desc);
  if (result) {
    sendall(conn_fd, result, strlen((char*)result), send_flags);
    free(result);
  }
  

  close(conn_fd);
}

static void sendall(int tcp_fd, void *buf, size_t length, int flags) {
  int incremental_sent;
  int total_sent = 0;
  while(total_sent < length) {
    incremental_sent = send(tcp_fd, buf + total_sent, length - total_sent, flags);
    if (incremental_sent <= 0) {
      fprintf(stderr, "Error sending buffer of len=%zu to fd=%d; sent=%d, errno=%d\n", 
          length, tcp_fd, total_sent, errno);
      return;
    }
    total_sent += incremental_sent;
  }
}

