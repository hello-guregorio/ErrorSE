#include "GenFileOffset.hh"

#include <filesystem>
#include <fstream>
#include <memory>
namespace fs = std::filesystem;
void GenNewsOffset(const char* newsFilePath, const char* outputFilePath) {
  if (fs::exists(fs::path(outputFilePath))) {
    return;
  }
  std::ifstream newsFile(newsFilePath, std::ios::binary);
  std::ofstream outputFile(outputFilePath);
  std::unique_ptr<char[]> buffer(std::make_unique<char[]>(1024 * 1024));
  int docid = 0;
  while (!newsFile.eof()) {
    int pre_offset = newsFile.tellg();
    newsFile.getline(buffer.get(), 1024 * 1024);
    if (!newsFile.eof()) {
      outputFile << docid++ << ' ' << pre_offset << '\n';
    }
  }
  newsFile.close();
  outputFile.close();
}