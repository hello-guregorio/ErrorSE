#include "Acceptor.hh"
#include "Noncopyable.hh"
#include <functional>
#include "TcpConnection.hh"
#include <map>
#include <mutex>
#include <sys/epoll.h>
using Functor = std::function<void()>;
class EventLoop : Noncopyable {
 public:
  EventLoop(Acceptor &);
  void Loop();
  void UnLoop();
  void runInEventLoop(Functor &&functor);  // 接收计算线程发送的IO操作函数

  void setConnectionCallBack(const TcpConnectionCallBack &);
  void setMessageCallBack(const TcpConnectionCallBack &);
  void setCloseCallBack(const TcpConnectionCallBack &);

 private:
  void epollWait();               // epoll_wait封装
  void addNewConnection(int fd);  // 监听到socket描述符，添加新的客户端连接
  void handleMessage(int fd);  // 监听到客户端描述符，处理信息
  int createEpollFd();         // 创建epfd
  int createEventFd();         // 创建eventfd
  void epollDel(int fd);       // epfd中删除fd的监控
  void epollAdd(int fd);       // epfd中增加fd的监控
  bool isClientQuit(int fd);   // 判断客户端是否退出
  void wakeup();               // 向_eventfd发送数据， 触发IO事件
  void handleRead();  //_eventfd响应之后再进行读取，防止一直响应
  void doPendingFunctors();  // 处理_functors中的函数

 private:
  int _epfd;                  // epoll 的描述符
  int _eventfd;               // 用于响应计算线程发回来的信号
  Acceptor &_acceptor;        // 用于得到服务器的socket与accept的newfd
  std::vector<epoll_event> _evs;   // 存放就绪的描述符
  std::vector<Functor> _functors;  // 用于存放待执行的IO操作函数
  std::mutex _mutexlock;       // 互斥锁
  std::map<int, TcpConnectionPtr> _tcpMap;  //<描述符，Tcp通信类>
  bool _isLoop;                        // epoll是否正在监测中

  TcpConnectionCallBack _connectionCB;
  TcpConnectionCallBack _messageCB;
  TcpConnectionCallBack _closeCB;
};