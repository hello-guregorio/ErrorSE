#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>

#include <string_view>

class InetAddress {
 public:
  InetAddress(std::string_view ip, uint16_t port);
  InetAddress(uint16_t port);
  InetAddress(const sockaddr_in& sai);
  sockaddr_in& GetSockAddr();
  std::string IP() const;
  uint16_t Port() const;

 private:
  sockaddr_in mSockAddr;
};