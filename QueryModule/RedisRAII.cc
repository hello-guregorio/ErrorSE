#include "RedisRAII.hh"

#include <fstream>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "BM25.hh"
#include "Jieba.hpp"
static const char* const DICT_PATH = "../ThirdParty/dict/jieba.dict.utf8";
static const char* const HMM_PATH = "../ThirdParty/dict/hmm_model.utf8";
static const char* const USER_DICT_PATH = "../ThirdParty/dict/user.dict.utf8";
static const char* const IDF_PATH = "../ThirdParty/dict/idf.utf8";
static const char* const STOP_WORD_PATH = "../ThirdParty/dict/stop_words.utf8";

class RedisRAIIImpl {
 public:
  RedisRAIIImpl()
      : jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH) {}
  redisContext* redisCTX;
  redisReply* reply;
  NewsOffsetLoader newsLoader;
  DictOffsetLoader dictLoader;
  cppjieba::Jieba jieba;
};

RedisRAII::RedisRAII(const char* ip, int port)
    : impl(std::make_unique<RedisRAIIImpl>()) {
  impl->redisCTX = redisConnect(ip, port);
}

RedisRAII::~RedisRAII() { redisFree(impl->redisCTX); }

void RedisRAII::StoreNewsOffset(const char* filePath) {
  std::ifstream inputFile(filePath);
  int docid, offset;
  while (inputFile >> docid) {
    inputFile >> offset;
    impl->reply = (redisReply*)redisCommand(
        impl->redisCTX, "HSET %s:DocId offset %s",
        std::to_string(docid).c_str(), std::to_string(offset).c_str());
    freeReplyObject(impl->reply);
  }
}

void RedisRAII::StoreDictOffset(const char* filePath) {
  std::ifstream inputFile(filePath);
  std::string term;
  int offset, df;
  while (inputFile >> term) {
    inputFile >> offset;
    inputFile >> df;
    impl->reply = (redisReply*)redisCommand(
        impl->redisCTX, "HSET %s:Term offset %s df %s", term.c_str(),
        std::to_string(offset).c_str(), std::to_string(df).c_str());
    freeReplyObject(impl->reply);
  }
}

void RedisRAII::StoreDocLen(const char* filePath) {
  std::ifstream inputFile(filePath);
  int docid;
  double doclen;
  while (inputFile >> docid) {
    inputFile >> doclen;
    impl->reply = (redisReply*)redisCommand(
        impl->redisCTX, "HSET %s:DocId doclen %s",
        std::to_string(docid).c_str(), std::to_string(doclen).c_str());
    freeReplyObject(impl->reply);
  }
}

// 如果不存在docid, 返回-1
int RedisRAII::GetNewsOffsetInfo(int docid) {
  impl->reply = (redisReply*)redisCommand(impl->redisCTX, "HGETALL %s:DocId",
                                          std::to_string(docid).c_str());
  if (impl->reply->elements == 0) {
    freeReplyObject(impl->reply);
    return -1;
  }
  int offset = atoi(impl->reply->element[1]->str);
  freeReplyObject(impl->reply);
  return offset;
}

double RedisRAII::GetDocLen(int docid) {
  impl->reply = (redisReply*)redisCommand(impl->redisCTX, "HGETALL %s:DocId",
                                          std::to_string(docid).c_str());
  if (impl->reply->elements == 0) {
    freeReplyObject(impl->reply);
    return -1;
  }
  double doclen = atof(impl->reply->element[3]->str);
  freeReplyObject(impl->reply);
  return doclen;
}

std::pair<int, int> RedisRAII::GetDictOffsetInfo(std::string_view str) {
  impl->reply =
      (redisReply*)redisCommand(impl->redisCTX, "HGETALL %s:Term", str.data());
  if (impl->reply->elements == 0) {
    freeReplyObject(impl->reply);
    return {-1, -1};
  }
  std::pair<int, int> ret{atoi(impl->reply->element[1]->str),
                          atoi(impl->reply->element[3]->str)};
  freeReplyObject(impl->reply);
  return ret;
}

std::string RedisRAII::SearchDoc(int docid) {
  int offset = GetNewsOffsetInfo(docid);
  if (offset == -1) {
    return "";
  }
  return impl->newsLoader.GetNewsFromOffset(offset);
}

std::vector<std::pair<int, int>> RedisRAII::SearchWord(std::string_view word) {
  auto [offset, df] = GetDictOffsetInfo(word);
  if (offset == -1) {
    return {};
  }
  return impl->dictLoader.GetPostingListFromOffset(offset, df);
}

std::vector<int> RedisRAII::SearchQuery(const std::string& query, int topK) {
  // std::vector<std::pair<std::string,int>> splitQuery(const std::string&
  // query) input query output queryVec
  std::vector<std::pair<std::string, int>> termtfPairs;
  std::vector<std::string> splitedTerms;
  impl->jieba.Cut(query, splitedTerms);
  std::sort(splitedTerms.begin(), splitedTerms.end());
  for (auto& term : splitedTerms) {
    if (termtfPairs.empty() || termtfPairs.back().first != term) {
      termtfPairs.emplace_back(term, 1);
    } else {
      termtfPairs.back().second++;
    }
  }
  return BM25(*this, termtfPairs, topK);
}
