#include <chrono>
#include <iostream>

#include "../IndexModule/BuildIndex.hh"
#include "../IndexModule/GenDocLenAndTuples.hh"
#include "../IndexModule/GenFileOffset.hh"
#include "../IndexModule/MergeSortedFile.hh"
#include "../IndexModule/NewsOffsetLoader.hh"
#include "../IndexModule/ParallelSort.hh"
#include "../IndexModule/SplitFile.hh"
#include "../IndexModule/WordSpliter.hh"
#include "../Server/ThreadPool.hh"
#include "../Server/Net/InetAddress.hh"
using namespace std::chrono;

class TinyTimer {
 public:
  TinyTimer() { start = system_clock::now(); }
  void PrintDelta() {
    auto end = system_clock::now();
    std::cout << "用时"
              << double(duration_cast<microseconds>(end - start).count()) *
                     microseconds::period::num / microseconds::period::den
              << "秒" << std::endl;
    start = end;
  }

 private:
  system_clock::time_point start;
};

int main() {
  // 生成news.offset, docid offset
  // docid是文档id, offset是这个文档在news.rawdata中的偏移位置
  // 由于不用新闻用'\n'隔开, 所以只需记录偏移
  TinyTimer timer;
  std::cout << "生成news.offset" << std::endl;
  GenNewsOffset();
  timer.PrintDelta();
  // 生成news.doclen和news.tps
  // news.doclen   docid   doclen文档长度
  // news.tps  <term, docid, tf>
  std::cout << "生成news.doclen和news.tps" << std::endl;
  GenDocLenAndTuples();
  GenAvgDocLen();
  timer.PrintDelta();
  // 这一步切分news.tps, 生成splited下的小文件
  std::cout << "切分文件" << std::endl;
  SplitFile();
  timer.PrintDelta();
  // 对splited下的每个小文件中的tuple排序, 生成的文件放入sorted文件夹
  std::cout << "小文件排序" << std::endl;
  ParallelSort();
  timer.PrintDelta();
  // 将sorted下有序的小文件合并成大文件news.sorted
  // news.sorted <term, docid, tf>
  std::cout << "合并文件" << std::endl;
  MergeSortedFiles();
  timer.PrintDelta();
  // 生成news.dict和news.dictoffset
  // news.dict <docid tf>
  // news.dictoffset   term  offset  lines  term对应的posting
  // list在news.dict中的偏移和行数
  std::cout << "构建索引" << std::endl;
  BuildIndex();
  timer.PrintDelta();


  //线程池测试
  // ThreadPool pool(4);
  // std::vector<std::future<int> > results;

  // for (int i = 0; i < 8; ++i) {
  //   results.emplace_back(pool.enqueue([i] {
  //     std::cout << "hello " << i << std::endl;
  //     std::this_thread::sleep_for(std::chrono::seconds(1));
  //     std::cout << "world " << i << std::endl;
  //     return i * i;
  //   }));
  // }

  // for (auto&& result : results) std::cout << result.get() << ' ';
  // std::cout << std::endl;

  //InetAddress测试
  // InetAddress ia("127.0.0.1",2333);
  // std::cout<<ia.IP()<<std::endl;
  // std::cout<<ia.Port()<<std::endl;
  return 0;
}