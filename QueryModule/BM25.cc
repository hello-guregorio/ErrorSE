#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "RedisRAII.hh"
// 运行时要从info.txt载入这两个
int NEWS_COUNT = 0;
double DOC_AVG_LEN = 0.0;

void LoadInfo(const char* filePath) {
  std::ifstream inputFile(filePath);
  std::string cfgName;
  while (inputFile >> cfgName) {
    if (cfgName == "NEWS_COUNT") {
      inputFile >> NEWS_COUNT;
    } else if (cfgName == "DOC_AVG_LEN") {
      inputFile >> DOC_AVG_LEN;
    }
  }
}

static inline double IDF(int df) {
  return log2(static_cast<double>(NEWS_COUNT - df + 0.5) / (df + 0.5));
}

// b k1 k3可调
static inline double WordWeight(int qtf, int tf, int doclen, int df,
                                double b = 0.75, double k1 = 1.5,
                                double k3 = 1.5) {
  double weight = IDF(df);
  double K = k1 * (1 - b + b * doclen / DOC_AVG_LEN);
  weight *= (k1 + 1) * tf / (K + tf);
  weight *= (k3 + 1) * qtf / (k3 + qtf);
  return weight;
}

static void QuickSelectHelper(std::vector<std::pair<int, double>>& vec,
                              int left, int right, int k) {
  if (left >= right) return;
  int l = left - 1, r = right + 1;
  double mid = vec[(l + r) >> 1].second;
  while (l < r) {
    while (vec[++l].second > mid)
      ;
    while (vec[--r].second < mid)
      ;
    if (l < r) {
      std::swap(vec[l], vec[r]);
    }
  }
  if (k <= r - left + 1) {
    QuickSelectHelper(vec, left, r, k);
  } else {
    QuickSelectHelper(vec, r + 1, right, k - (r - left + 1));
  }
}

static void QuickSelect(std::vector<std::pair<int, double>>& vec, int k) {
  QuickSelectHelper(vec, 0, vec.size() - 1, k);
}

// 在排序前需要将所有文档的长度载入redis
std::vector<int> BM25(RedisRAII& redisRAII,
                      std::vector<std::pair<std::string, int>>& queryVec,
                      int topK) {
  // key: docid value:score
  std::unordered_map<int, double> docScore;
  for (auto& [term, qtf] : queryVec) {
    // get docid tf
    auto postingList = redisRAII.SearchWord(term);
    // 倒排索引没有这个term对应权重就是0, 第二个for循环执行次数变为0
    int df = postingList.size();
    for (auto& [docid, tf] : postingList) {
      double doclen = redisRAII.GetDocLen(docid);
      docScore[docid] += WordWeight(qtf, tf, doclen, df);
    }
  }
  std::vector<std::pair<int, double>> vec;
  vec.reserve(docScore.size());
  for (auto& [k, v] : docScore) {
    vec.emplace_back(k, v);
  }
  // 拿前topK
  QuickSelect(vec, topK);
  std::vector<int> ret(topK);
  for (int i = 0; i < topK; ++i) {
    ret[i] = vec[i].first;
    std::cout << vec[i].second << std::endl;
  }
  return ret;
}