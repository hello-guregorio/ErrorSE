#include "MergeSortedFile.hh"

#include <filesystem>
#include <fstream>
#include <queue>

namespace fs = std::filesystem;

using Term = std::string;
using DocId = int;
using TermFrequency = int;
using tp = std::tuple<Term, DocId, TermFrequency, int>;

void MergeSortedFiles(const char* dirPath, const char* outputPath) {
  if (fs::exists(fs::path(outputPath))) {
    return;
  }
  fs::path dirFsPath{dirPath};
  std::vector<std::ifstream> vec;
  for (auto& fentry : fs::directory_iterator(dirFsPath)) {
    auto fpath = fentry.path();
    vec.emplace_back(fpath);
  }
  std::priority_queue<tp, std::vector<tp>, std::greater<tp>> pq;
  Term term;
  DocId docid;
  TermFrequency tf;
  for (size_t i = 0; i < vec.size(); ++i) {
    vec[i] >> term >> docid >> tf;
    pq.emplace(term, docid, tf, i);
  }
  std::ofstream outputFile(outputPath);
  while (!pq.empty()) {
    auto [term, docid, tf, ifpos] = pq.top();
    outputFile << term << ' ' << docid << ' ' << tf << '\n';
    pq.pop();
    // 看看是否还能push
    std::string tmp;
    vec[ifpos] >> tmp;
    if (tmp == "") continue;
    DocId tmp2;
    TermFrequency tmp3;
    vec[ifpos] >> tmp2;
    vec[ifpos] >> tmp3;
    pq.emplace(tmp, tmp2, tmp3, ifpos);
  }
  outputFile.close();
  for (auto& v : vec) {
    v.close();
  }
}