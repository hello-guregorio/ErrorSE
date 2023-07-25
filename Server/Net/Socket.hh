#pragma once
class Socket {
 public:
  Socket();
  explicit Socket(int fd);
  ~Socket();
  int getFd() const;
  void shutdown_write();

 private:
  int _fd;
};