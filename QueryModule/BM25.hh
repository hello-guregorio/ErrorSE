#pragma once
#include <vector>

#include "RedisRAII.hh"
extern int NEWS_COUNT;
extern double DOC_AVG_LEN;

void LoadInfo(const char* filePath = "../data/info.txt");

std::vector<int> BM25(RedisRAII& redisRAII,
                      std::vector<std::pair<std::string, int>>& queryVec,
                      int topK = 10);