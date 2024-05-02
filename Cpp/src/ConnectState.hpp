#ifndef CONNECTSTATE_HPP
#define CONNECTSTATE_HPP

#include "meta.hpp" // Include GameMeta structure
#include <vector>

class ConnectState {
private:
    std::vector<std::vector<int>> board;
    std::vector<int> height;
    std::vector<int> last_played;

public:
    int to_play;
    ConnectState();
    std::vector<std::vector<int>> get_board();
    void move(int col);
    std::vector<int> get_legal_moves();
    int check_win();
    bool check_win_from(int row, int col);
    bool game_over();
    int get_outcome();
    void print();
    int get_to_play() const {
        return to_play;
    }
};

#endif
