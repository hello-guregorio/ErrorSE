#include "InetAddress.hh"

#include <cstring>
#include <string>
InetAddress::InetAddress(std::string_view ip, uint16_t port) {
  memset(&mSockAddr, 0, sizeof(mSockAddr));
  mSockAddr.sin_family = AF_INET;
  mSockAddr.sin_port = htons(port);
  mSockAddr.sin_addr.s_addr = inet_addr(ip.data());
}

InetAddress::InetAddress(uint16_t port) {
  memset(&mSockAddr, 0, sizeof(mSockAddr));
  mSockAddr.sin_family = AF_INET;
  mSockAddr.sin_addr.s_addr = INADDR_ANY;
}

InetAddress::InetAddress(const sockaddr_in& sai) {
  mSockAddr = sai;
}

sockaddr_in& InetAddress::GetSockAddr() { return this->mSockAddr; }

std::string InetAddress::IP() const {
  return std::string(inet_ntoa(mSockAddr.sin_addr));
}

uint16_t InetAddress::Port() const { return ntohs(mSockAddr.sin_port); }
