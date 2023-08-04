#include <iostream>

#include "../QueryModule/RedisRAII.hh"
#include "Net/TcpServer.hh"
#include "ThreadPool.hh"

ThreadPool* tpPtr = nullptr;
void ConnectionCallBack(const TcpConnectionPtr& ptr) {
  std::cout << ">>client has connected" << ptr->Address() << std::endl;
}

static void DoQuery(const std::string& msg, const TcpConnectionPtr& tcpPtr) {
  static RedisRAII instance;
  std::vector docIds = instance.SearchQuery(msg);
  std::string vecStr = "{";
  for (int id : docIds) {
    vecStr.append(std::to_string(id) + ", ");
  }
  vecStr.push_back('}');
  tcpPtr->sendInEventLoop(vecStr);
}

void MessageCallBack(const TcpConnectionPtr& ptr) {
  std::string msg(ptr->recv());
  std::cout << "client:" << msg << ",size:" << msg.size() << std::endl;
  tpPtr->enqueue(DoQuery, msg, ptr);
}

void CloseCallBack(const TcpConnectionPtr& ptr) {
  std::cout << ">>client has broken up" << ptr->Address() << std::endl;
}

int main() {
  ThreadPool threadpool(10);
  tpPtr = &threadpool;
  TcpServer server("127.0.0.1", 2333);
  server.setConnectionCallBack(ConnectionCallBack);
  server.setMessageCallBack(MessageCallBack);
  server.setCloseCallBack(CloseCallBack);
}