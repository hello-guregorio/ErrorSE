# 构建
cmake -S . -B build
cmake --build build
# 测试
对316.9MiB的文件构建倒排索引各个步骤用时如下:

生成news.offset 用时0.080493秒

生成news.doclen和news.tps 用时204.048秒(瓶颈, 后续想办法优化)

切分文件 用时2.26755秒

小文件排序 用时12.1876秒

合并文件 用时68.0306秒

构建索引 用时9.91235秒
# data下各个文件作用
- news.rawdata  二进制存储, 每行一个json格式的新闻
- newscount.txt 记录新闻爬取个数