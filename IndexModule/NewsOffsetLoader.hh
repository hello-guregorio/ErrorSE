#pragma once
#include <fstream>
#include <string>
class NewsOffsetLoader {
 public:
  NewsOffsetLoader(const char* filePath = "../data/news.rawdata")
      : inputFile(filePath, std::ios::binary) {}
  ~NewsOffsetLoader() { inputFile.close(); }
  
  std::string GetNewsFromOffset(int offset) {
    std::string ret;
    inputFile.seekg(offset);
    std::getline(inputFile, ret);
    return ret;
  }

 private:
  std::ifstream inputFile;
};