#pragma once
#include <functional>
#include <memory>

#include "InetAddress.hh"
#include "Noncopyable.hh"
#include "Socket.hh"
#include "SocketIO.hh"
class EventLoop;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using TcpConnectionCallBack = std::function<void(const TcpConnectionPtr&)>;

class TcpConnection : Noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(int fd, EventLoop*);
  ~TcpConnection();
  std::string recv();
  void send(const std::string& s);
  void shutdownWrite();
  std::string Address() const;
  void sendInEventLoop(
      const std::string& msg);  // 将消息重新发回EventLoop中，让IO线程发送

  void setConnectionCallBack(const TcpConnectionCallBack&);
  void setMessageCallBack(const TcpConnectionCallBack&);
  void setCloseCallBack(const TcpConnectionCallBack&);

  void handleConnectionCallBack();
  void handleMessageCallBack();
  void handleCloseCallBack();

 private:
  InetAddress getLocal(int);
  InetAddress getPeer(int);

 private:
  Socket _socket;
  SocketIO _sio;
  InetAddress _localAddress;
  InetAddress _peerAddress;
  bool _isShutdownWrite;
  EventLoop* _pEventLoop;

  TcpConnectionCallBack _connectionCB;
  TcpConnectionCallBack _messageCB;
  TcpConnectionCallBack _closeCB;
};