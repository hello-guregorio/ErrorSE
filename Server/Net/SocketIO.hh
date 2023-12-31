#pragma once
#include "Socket.hh"

class SocketIO {
 public:
  SocketIO(int sockfd);
  ~SocketIO();
  int readn(void* buf, int len);
  int writen(const char* buf, int len);
  int readline(void* buf, int len);

 private:
  int recvPeek(char* buf, int len);
  Socket _socket;
};