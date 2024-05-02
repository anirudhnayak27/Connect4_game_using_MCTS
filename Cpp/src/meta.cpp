#include "meta.hpp"
#include <iostream>
#include <unordered_map>
#include <limits>

#include <string>

const std::unordered_map<std::string, int> GameMeta::PLAYERS = {
    {"none", 0},
    {"one", 1},
    {"two", 2}
};

const std::unordered_map<std::string, int> GameMeta::OUTCOMES = {
    {"none", 0},
    {"one", 1},
    {"two", 2},
    {"draw", 3}
};

const double GameMeta::INF = std::numeric_limits<double>::infinity();
const int GameMeta::ROWS = 6;
const int GameMeta::COLS = 7;

const double MCTSMeta::EXPLORATION = std::sqrt(2);
