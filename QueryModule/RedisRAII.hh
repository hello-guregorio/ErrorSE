#pragma once
#include <hiredis/hiredis.h>

#include <string_view>
#include <utility>
#include <memory>
#include "../IndexModule/DictOffsetLoader.hh"
#include "../IndexModule/NewsOffsetLoader.hh"

class RedisRAIIImpl;

class RedisRAII {
 public:
  RedisRAII(const char* ip = "127.0.0.1", int port = 6379);
  ~RedisRAII();
  // 下面三个函数调用一次即可
  //  将news.dict存入redis
  void StoreNewsOffset(const char* filePath = "../data/news.offset");
  // 将news.dictoffset存入redis
  void StoreDictOffset(const char* filePath = "../data/news.dictoffset");
  // 将news.doclen传入redis
  void StoreDocLen(const char* filePath = "../data/news.doclen");

  // 传入docid, 返回在news.rawdata中偏移
  int GetNewsOffsetInfo(int docid);
  // 传入docid, 返回在文档的长度
  double GetDocLen(int docid);
  // 传入词, 返回词的offset df
  std::pair<int, int> GetDictOffsetInfo(std::string_view str);

  std::string SearchDoc(int docid);
  // docid tf (df就是这个vector的长度)
  std::vector<std::pair<int, int>> SearchWord(std::string_view word);

  //使用bm25算法查询 返回topk的文档id
  std::vector<int> SearchQuery(const std::string& query,int topK=10);
 private:
  std::unique_ptr<RedisRAIIImpl> impl;
};