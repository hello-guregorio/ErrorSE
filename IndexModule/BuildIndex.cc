#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;
void BuildIndex(const char* sortedFilePath, const char* indexFilePath,
                const char* indexOffsetFilePath) {
  if (fs::exists(fs::path(indexFilePath))) {
    return;
  }
  int docid;
  std::string term;
  int tf;
  std::string pre;
  std::ifstream sortedFile(sortedFilePath);
  std::ofstream indexOffsetFile(indexOffsetFilePath);
  std::ofstream indexFile(indexFilePath);
  // 存临时倒排记录
  // docid tf
  std::vector<std::pair<int, int>> prVec;
  while (sortedFile >> term) {
    sortedFile >> docid;
    sortedFile >> tf;
    if (pre != term && prVec.size() != 0) {
      // term offset(在索引文件中的偏移) 行数
      indexOffsetFile << pre << ' ' << indexFile.tellp() << ' ' << prVec.size()
                      << '\n';
      for (auto& [t1, t2] : prVec) {
        // docid tf
        indexFile << t1 << ' ' << t2 << '\n';
      }
      prVec.clear();
      pre = term;
    }
    prVec.emplace_back(docid, tf);
    pre = term;
  }
  indexOffsetFile << pre << ' ' << indexFile.tellp() << ' ' << prVec.size()
                  << '\n';
  for (auto& [t1, t2] : prVec) {
    indexFile << t1 << ' ' << t2 << '\n';
  }
  sortedFile.close();
  indexOffsetFile.close();
  indexFile.close();
}