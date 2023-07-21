#include "ParallelSort.hh"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
namespace fs = std::filesystem;
static const char* sortedPath = "../data/sorted/";

/// @brief 排序任务
/// @param start 开始文件位置
/// @param end 结束文件位置
/// @param filesPath 文件路径数组
/// 注意左闭右开
static void Task(int start, int end,
                 std::vector<fs::directory_entry>& filesPath) {
  std::vector<std::tuple<std::string, int, int>> tpsVec;
  std::string term;
  int docid;
  int tf;
  for (int i = start; i < end; ++i) {
    std::ifstream inputFile(filesPath[i].path().c_str());
    while (!inputFile.eof()) {
      inputFile >> term >> docid >> tf;
      if (inputFile.eof()) {
        break;
      }
      tpsVec.emplace_back(term, docid, tf);
    }
    std::sort(tpsVec.begin(), tpsVec.end(),
              // term docid tf
              std::less<std::tuple<std::string, int, int>>());
    std::string curSortedPathName =
        sortedPath + static_cast<std::string>(filesPath[i].path().filename());
    std::ofstream outputFile(curSortedPathName);
    for (size_t i = 0; i < tpsVec.size(); ++i) {
      auto& [t1, t2, t3] = tpsVec[i];
      outputFile << t1 << ' ' << t2 << ' ' << t3 << '\n';
    }
    tpsVec.clear();
    inputFile.close();
    outputFile.close();
  }
}

// 线程数
// 任务数
void ParallelSortHelper(int threadNum,
                        std::vector<fs::directory_entry>& filesPath) {
  std::vector<std::thread> threads;
  int taskNum = filesPath.size();
  // 各个线程要处理的文件数
  std::vector<int> tasksNum(threadNum, taskNum / threadNum);
  for (int i = 0; i < taskNum % threadNum; ++i) {
    tasksNum[i]++;
  }
  int startPos = 0;
  for (int i = 0; i < threadNum; ++i) {
    threads.emplace_back(Task, startPos, startPos + tasksNum[i],
                         std::ref(filesPath));
    startPos += tasksNum[i];
  }
  for (int i = 0; i < threadNum; ++i) {
    threads[i].join();
  }
}

void ParallelSort(int threadNum, const char* dir) {
  if (fs::exists(fs::path(sortedPath))) {
    return;
  } else {
    fs::create_directory(fs::path(sortedPath));
  }
  fs::path dirPath(dir);
  std::vector<fs::directory_entry> filesPath;
  for (auto& dentry : fs::directory_iterator(dirPath)) {
    filesPath.push_back(dentry);
  }
  ParallelSortHelper(threadNum, filesPath);
}