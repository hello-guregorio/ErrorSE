#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>

#include <string>

using std::string;
class InetAddress {
 public:
  InetAddress(const string& Ip, unsigned short Port);
  InetAddress(unsigned short Port);
  InetAddress(const sockaddr_in& ser);
  sockaddr_in* getAddressPtr();
  string getIp() const;
  unsigned short getPort() const;

 private:
  sockaddr_in _ser;
};