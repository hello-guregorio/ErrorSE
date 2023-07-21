#include "SplitFile.hh"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

void SplitFile(const char* filePath, const char* outputPath, int records) {
  if (fs::exists(fs::path(outputPath))) {
    return;
  } else {
    fs::create_directory(fs::path(outputPath));
  }
  std::ifstream inputFile(filePath);
  std::string buff;
  std::string outPath = std::string(outputPath) + "/piece";
  int pieceNum = 0;
  int numStartPos = outPath.size();
  while (!inputFile.eof()) {
    outPath.append(std::to_string(pieceNum++));
    std::ofstream outputFile(outPath.c_str());
    for (int i = 0; i < records && !inputFile.eof(); ++i) {
      std::getline(inputFile, buff);
      if (buff.size() == 0) {
        break;
      }
      outputFile << buff << '\n';
    }
    outputFile.close();
    outPath.erase(outPath.begin() + numStartPos, outPath.end());
  }
  inputFile.close();
}