#ifndef CONNECT_4_H
#define CONNECT_4_H

#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <limits>
#include <random>
#include <chrono>
#include <algorithm>

class GameMeta {
public:
    static const std::unordered_map<std::string, int> PLAYERS;
    static const std::unordered_map<std::string, int> OUTCOMES;
    static constexpr double INF = std::numeric_limits<double>::infinity();
    static const int ROWS = 6;
    static const int COLS = 7;
};

class MCTSMeta {
public:
    static const double EXPLORATION;
};

class ConnectState {
    public: 
        ConnectState();
        std::vector<std::vector<int>> get_board() const;
        void move(int col);
        std::vector<int> get_legal_moves() const;
        int check_win();
        bool check_win_from(int row, int col);
        bool game_over();
        int get_outcome();
        void print() const;

    private:
        std::vector<std::vector<int>> board;
        int to_play;
        std::vector<int> height;
        std::vector<int> last_played;
};

class Node {
public:
    Node(int move, Node* parent);
    void add_children(const std::vector<Node*>& children);
    double value(double explore = MCTSMeta::EXPLORATION) const;

    int move;
    Node* parent;
    int N;
    double Q;
    std::unordered_map<int, Node*> children;
    int outcome;
};
class MCTS {
public:
    MCTS(const ConnectState& state);
    ~MCTS();
    void set_max_rollouts(int max_rollouts);
    std::pair<Node*, ConnectState> select_node();
    bool expand(Node* parent, ConnectState& state);
    int roll_out(const ConnectState& state);
    void back_propagate(Node* node, int turn, int outcome);
    void search();
    int best_move();
    void move(int move);
    std::pair<int, int> statistics() const;

private:
    void deleteNodes(Node* node);
    ConnectState root_state;
    Node* root;
    int num_rollouts;
    int run_time;
    int max_rollouts;
};

void play();

#endif 