#include "Noncopyable.hh"
#include <string>
#include "TcpConnection.hh"
#include "Acceptor.hh"
#include "EventLoop.hh"
using std::string;
class TcpServer : Noncopyable {
 public:
  TcpServer(const string &Ip, unsigned short port);
  void start();
  void setConnectionCallBack(const TcpConnectionCallBack &);
  void setMessageCallBack(const TcpConnectionCallBack &);
  void setCloseCallBack(const TcpConnectionCallBack &);

 private:
  Acceptor _acceptor;
  EventLoop _eventLoop;
};