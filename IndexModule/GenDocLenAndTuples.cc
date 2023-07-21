#include "GenDocLenAndTuples.hh"

#include <filesystem>
#include <iostream>
#include <thread>

#include "NewsOffsetLoader.hh"
#include "WordSpliter.hh"
namespace fs = std::filesystem;

void GenDocLenAndTuples(const char* offsetPath, const char* tpsPath,
                        const char* doclenPath) {
  if (fs::exists(fs::path(tpsPath))) {
    return;
  }
  NewsOffsetLoader loader;
  std::ifstream offsetFile(offsetPath);
  std::ofstream tpsFile(tpsPath);
  std::ofstream doclenFile(doclenPath);

  while (!offsetFile.eof()) {
    int docid, offset;
    offsetFile >> docid;
    offsetFile >> offset;
    if (offsetFile.eof()) {
      break;
    }
    std::string newsJson = loader.GetNewsFromOffset(offset);
    auto [_, doclen, termtfPairs] = SplitWord(newsJson);
    for (auto& [term, tf] : termtfPairs) {
      tpsFile << term << ' ' << docid << ' ' << tf << '\n';
    }
    doclenFile << docid << ' ' << doclen << '\n';
  }
  doclenFile.close();
  tpsFile.close();
  offsetFile.close();
}