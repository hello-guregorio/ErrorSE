#pragma once
#include <string_view>
#include <tuple>
#include <vector>


// docid doclen vec<pair<term, tf>>
std::tuple<int, double, std::vector<std::pair<std::string, int>>> SplitWord(
    std::string_view newsJson);