#ifndef META_HPP
#define META_HPP

#include <cmath>
#include <unordered_map>
#include <string>
class GameMeta {
public:
    static const std::unordered_map<std::string, int> PLAYERS;
    static const std::unordered_map<std::string, int> OUTCOMES;
    static const double INF;
    static const int ROWS;
    static const int COLS;
};

class MCTSMeta {
public:
    static const double EXPLORATION;
};

#endif
