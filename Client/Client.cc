#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <climits>
#include <iostream>
using namespace std;

int main() {
  int ret;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket");
    exit(-1);
  }
  struct sockaddr_in sockinfo;
  bzero(&sockinfo, sizeof(sockinfo));
  sockinfo.sin_addr.s_addr = inet_addr("172.21.0.7");
  sockinfo.sin_port = htons(8888);
  sockinfo.sin_family = AF_INET;
  ret = connect(sockfd, (sockaddr *)&sockinfo, sizeof(sockinfo));
  if (-1 == ret) {
    perror("connect");
    exit(-1);
  }

  char buf[1000000];

  while (1) {
    bzero(buf, sizeof(buf));
    read(0, buf, sizeof(buf));
    send(sockfd, buf, strlen(buf) - 1, 0);

    bzero(buf, sizeof(buf));
    recv(sockfd, buf, sizeof(buf), 0);
    cout << buf << endl;
  }

  close(sockfd);
  return 0;
}