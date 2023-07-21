#pragma once

void GenDocLenAndTuples(const char* offsetPath = "../data/news.offset",
                        const char* tpsPath = "../data/news.tps",
                        const char* doclenPath = "../data/news.doclen");

void GenAvgDocLen(const char* infoPath = "../data/info.txt");