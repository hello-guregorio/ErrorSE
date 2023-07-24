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

void GenAvgDocLen(const char* infoPath) {
  std::fstream infoFile(infoPath, std::ios::app | std::ios::in | std::ios::out);
  std::string name;
  int fileCount;
  infoFile >> name >> fileCount;
  if (!infoFile.eof()) {
    return;
  }
  std::ifstream doclenFile("../data/news.doclen");
  int docid;
  double len;
  double docAVGLen = 0;
  while (doclenFile >> docid) {
    doclenFile >> len;
    docAVGLen += len;
  }
  docAVGLen /= static_cast<double>(fileCount);
  infoFile << "DOC_AVG_LEN " << docAVGLen << '\n';
  infoFile.close();
  doclenFile.close();
}