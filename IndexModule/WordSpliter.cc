#include "WordSpliter.hh"

#include <cmath>
#include <regex>

#include "Jieba.hpp"
#include "json.hpp"
static const char* const DICT_PATH = "../ThirdParty/dict/jieba.dict.utf8";
static const char* const HMM_PATH = "../ThirdParty/dict/hmm_model.utf8";
static const char* const USER_DICT_PATH = "../ThirdParty/dict/user.dict.utf8";
static const char* const IDF_PATH = "../ThirdParty/dict/idf.utf8";
static const char* const STOP_WORD_PATH = "../ThirdParty/dict/stop_words.utf8";

using json = nlohmann::json;

static cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH,
                             STOP_WORD_PATH);
// 转换成unicode用,准备过滤特殊字符
static std::wstring_convert<std::codecvt_utf8<wchar_t>> wstringCVT;
static std::wregex rule(L"[^a-zA-z\\d\u4E00-\u9FA5]");

std::vector<std::pair<std::string, int>> SplitWord(
    const std::string& needSplit) {
  std::wstring ws = wstringCVT.from_bytes(needSplit);
  std::string regexedStr =
      wstringCVT.to_bytes(std::regex_replace(ws, rule, L""));
  std::vector<std::string> splitedTerms;
  jieba.CutForSearch(regexedStr, splitedTerms);
  std::sort(splitedTerms.begin(), splitedTerms.end());
  // 生成term, tf对
  std::vector<std::pair<std::string, int>> termtfPairs;
  for (auto& term : splitedTerms) {
    if (termtfPairs.empty() || termtfPairs.back().first != term) {
      termtfPairs.emplace_back(term, 1);
    } else {
      termtfPairs.back().second++;
    }
  }
  return termtfPairs;
}

std::tuple<int, double, std::vector<std::pair<std::string, int>>> SplitWord(
    std::string_view newsJson) {
  std::string needSplit;
  json jsonDict = json::parse(newsJson);
  int docid = jsonDict["docid"];
  needSplit.append(jsonDict["title"]);
  needSplit.append(jsonDict["content"]);
  // 切分词
  auto termtfPairs = SplitWord(needSplit);
  // 计算doclen
  double doclen = 0;
  for (auto& [_, tf] : termtfPairs) {
    doclen += tf * tf;
  }
  doclen = sqrt(doclen);
  return {docid, doclen, termtfPairs};
}
