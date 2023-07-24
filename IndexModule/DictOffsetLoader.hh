#pragma once
#include <fstream>
#include <vector>

class DictOffsetLoader {
 public:
  DictOffsetLoader(const char* filePath = "../data/news.dict")
      : inputFile(filePath) {}
  // docid tf
  std::vector<std::pair<int, int>> GetPostingListFromOffset(int offset,
                                                            int lines) {
    inputFile.seekg(offset);
    std::vector<std::pair<int, int>> vec;
    vec.reserve(lines);
    int docid, tf;
    for (int i = 0; i < lines; ++i) {
      inputFile >> docid >> tf;
      vec.emplace_back(docid, tf);
    }
    return vec;
  }

 private:
  std::ifstream inputFile;
};