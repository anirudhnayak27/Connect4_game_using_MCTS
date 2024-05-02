#ifndef MCTS_HPP
#define MCTS_HPP

#include "ConnectState.hpp"
#include "meta.hpp"
#include <unordered_map>
#include <random>
#include <cmath>
#include <ctime>
#include <chrono>

class Node {
public:
    int move;
    Node* parent;
    int N;
    double Q;
    std::unordered_map<int, Node*> children;
    int outcome;

    Node(int move, Node* parent) : move(move), parent(parent), N(0), Q(0), outcome(GameMeta::PLAYERS.at("none")) {}

    void add_children(std::unordered_map<int, Node*>& children) {
        for (auto& [key, value] : children) {
            this->children[key] = value;
        }
    }

    double value(double explore = MCTSMeta::EXPLORATION) {
        if (N == 0) {
            return (explore == 0) ? 0 : GameMeta::INF;
        } else {
            return Q / N + explore * sqrt(log(parent->N) / N);
        }
    }
};

class MCTS {
private:
    ConnectState root_state;
    Node* root;
    double run_time;
    int node_count;
    int num_rollouts;

    Node* select_node();
    bool expand(Node* parent, ConnectState& state);
    int roll_out(ConnectState& state);
    void back_propagate(Node* node, int turn, int outcome);

public:
    MCTS(const ConnectState& state);
    void search(int time_limit);
    int best_move();
    void move(int move);
    std::pair<int, double> statistics();
};

#endif
