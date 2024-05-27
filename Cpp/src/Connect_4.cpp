#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <limits>
#include <random>
#include <chrono>
#include <algorithm>
#include "connect_4.h"

// GameMeta class implementation
const std::unordered_map<std::string, int> GameMeta::PLAYERS = {
    {"none", 0}, {"one", 1}, {"two", 2}
};

const std::unordered_map<std::string, int> GameMeta::OUTCOMES = {
    {"none", 0}, {"one", 1}, {"two", 2}, {"draw", 3}
};

// MCTSMeta class implementation
const double MCTSMeta::EXPLORATION = std::sqrt(2.0);

// ConnectState class implementation
ConnectState::ConnectState() {
    board = std::vector<std::vector<int>>(GameMeta::ROWS, std::vector<int>(GameMeta::COLS, 0));
    to_play = GameMeta::PLAYERS.at("one");
    height = std::vector<int>(GameMeta::COLS, GameMeta::ROWS - 1);
}

std::vector<std::vector<int>> ConnectState::get_board() const {
    return board;
}

void ConnectState::move(int col) {
    board[height[col]][col] = to_play;
    last_played = {height[col], col};
    height[col]--;
    to_play = (to_play == GameMeta::PLAYERS.at("one")) ? GameMeta::PLAYERS.at("two") : GameMeta::PLAYERS.at("one");
}

std::vector<int> ConnectState::get_legal_moves() const {
    std::vector<int> legal_moves;
    for (int col = 0; col < GameMeta::COLS; ++col) {
        if (board[0][col] == 0) {
            legal_moves.push_back(col);
        }
    }
    return legal_moves;
}

int ConnectState::check_win() {
    if (!last_played.empty() && check_win_from(last_played[0], last_played[1])) {
        return board[last_played[0]][last_played[1]];
    }
    return 0;
}

bool ConnectState::check_win_from(int row, int col) {
    int player = board[row][col];

        // Check horizontal
        int consecutive = 1;
        int tmprow = row;
        while (tmprow + 1 < GameMeta::ROWS && board[tmprow + 1][col] == player) {
            consecutive++;
            tmprow++;
        }
        tmprow = row;
        while (tmprow - 1 >= 0 && board[tmprow - 1][col] == player) {
            consecutive++;
            tmprow--;
        }
        if (consecutive >= 4) {
            return true;
        }

        // Check vertical
        consecutive = 1;
        int tmpcol = col;
        while (tmpcol + 1 < GameMeta::COLS && board[row][tmpcol + 1] == player) {
            consecutive++;
            tmpcol++;
        }
        tmpcol = col;
        while (tmpcol - 1 >= 0 && board[row][tmpcol - 1] == player) {
            consecutive++;
            tmpcol--;
        }
        if (consecutive >= 4) {
            return true;
        }

        // Check diagonal
        consecutive = 1;
        tmprow = row;
        tmpcol = col;
        while (tmprow + 1 < GameMeta::ROWS && tmpcol + 1 < GameMeta::COLS && board[tmprow + 1][tmpcol + 1] == player) {
            consecutive++;
            tmprow++;
            tmpcol++;
        }
        tmprow = row;
        tmpcol = col;
        while (tmprow - 1 >= 0 && tmpcol - 1 >= 0 && board[tmprow - 1][tmpcol - 1] == player) {
            consecutive++;
            tmprow--;
            tmpcol--;
        }
        if (consecutive >= 4) {
            return true;
        }

        // Check anti-diagonal
        consecutive = 1;
        tmprow = row;
        tmpcol = col;
        while (tmprow + 1 < GameMeta::ROWS && tmpcol - 1 >= 0 && board[tmprow + 1][tmpcol - 1] == player) {
            consecutive++;
            tmprow++;
            tmpcol--;
        }
        tmprow = row;
        tmpcol = col;
        while (tmprow - 1 >= 0 && tmpcol + 1 < GameMeta::COLS && board[tmprow - 1][tmpcol + 1] == player) {
            consecutive++;
            tmprow--;
            tmpcol++;
        }
        if (consecutive >= 4) {
            return true;
        }

        return false;
}

bool ConnectState::game_over() {
    return check_win() != 0 || get_legal_moves().empty();
}

int ConnectState::get_outcome() {
    if (get_legal_moves().empty() && check_win() == 0) {
        return GameMeta::OUTCOMES.at("draw");
    }
    return (check_win() == GameMeta::PLAYERS.at("one")) ? GameMeta::OUTCOMES.at("one") : GameMeta::OUTCOMES.at("two");
}

void ConnectState::print() const {
    std::cout << "=============================\n";
    for (int row = 0; row < GameMeta::ROWS; ++row) {
        for (int col = 0; col < GameMeta::COLS; ++col) {
            std::cout << "| " << ((board[row][col] == 1) ? 'X' : (board[row][col] == 2) ? 'O' : ' ') << " ";
        }
        std::cout << "|\n";
    }
    std::cout << "=============================\n";
}

// Node class implementation
Node::Node(int move, Node* parent)
    : move(move), parent(parent), N(0), Q(0), outcome(GameMeta::PLAYERS.at("none")) {}

void Node::add_children(const std::vector<Node*>& children) {
    for (Node* child : children) {
        this->children[child->move] = child;
    }
}

double Node::value(double explore) const {
    if (N == 0) {
        return (explore == 0) ? 0 : GameMeta::INF;
    } else {
        return Q / static_cast<double>(N) + explore * std::sqrt(std::log(static_cast<double>(parent->N)) / static_cast<double>(N));
    }
}

// MCTS class implementation
MCTS::MCTS(const ConnectState& state) : root_state(state) {
    root = new Node(-1, nullptr);
}

MCTS::~MCTS() {
    deleteNodes(root);
}

void MCTS::set_max_rollouts(int max_rollouts) {
    this->max_rollouts = max_rollouts;
}

std::pair<Node*, ConnectState> MCTS::select_node() {
    Node* node = root;
    ConnectState state = root_state;

    while (!node->children.empty()) {
        std::vector<Node*> children;
        for (const auto& child : node->children) {
            children.push_back(child.second);
        }

        auto max_node = std::max_element(children.begin(), children.end(), [](const Node* a, const Node* b) {
            return a->value() < b->value();
        });
        double max_value = (*max_node)->value();

        std::vector<Node*> max_nodes;
        for (Node* child : children) {
            if (child->value() == max_value) {
                max_nodes.push_back(child);
            }
        }

        node = max_nodes[rand() % max_nodes.size()];
        state.move(node->move);

        if (node->N == 0) {
            return std::make_pair(node, state);
        }
    }

    if (expand(node, state)) {
        node = node->children.begin()->second;
        state.move(node->move);
    }

    return std::make_pair(node, state);
}

bool MCTS::expand(Node* parent, ConnectState& state) {
    if (state.game_over()) {
        return false;
    }

    std::vector<Node*> children;
    for (int move : state.get_legal_moves()) {
        children.push_back(new Node(move, parent));
    }
    parent->add_children(children);

    return true;
}

int MCTS::roll_out(const ConnectState& state) {
    ConnectState rollout_state = state;
    while (!rollout_state.game_over()) {
        std::vector<int> legal_moves = rollout_state.get_legal_moves();
        rollout_state.move(legal_moves[rand() % legal_moves.size()]);
    }
    return rollout_state.get_outcome();
}

void MCTS::back_propagate(Node* node, int turn, int outcome) {
    int reward = (outcome == turn) ? 0 : 1;

    while (node != nullptr) {
        node->N++;
        node->Q += reward;
        node = node->parent;
        if (outcome == GameMeta::OUTCOMES.at("draw")) {
            reward = 0;
        } else {
            reward = 1 - reward;
        }
    }
}

void MCTS::search() {
    auto start_time = std::chrono::steady_clock::now();

    num_rollouts = 0;
    while (!root_state.game_over() && num_rollouts < max_rollouts) {
        Node* node;
        ConnectState state;
        std::tie(node, state) = select_node();
        int outcome = roll_out(state);
        back_propagate(node, state.get_legal_moves().empty() ? GameMeta::PLAYERS.at("none") : state.get_legal_moves()[0], outcome);
        num_rollouts++;
    }

    run_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
}

int MCTS::best_move() {
    if (root_state.game_over()) {
        return -1;
    }

    int max_visits = std::max_element(root->children.begin(), root->children.end(), [](const std::pair<int, Node*>& a, const std::pair<int, Node*>& b) {
        return a.second->N < b.second->N;
    })->second->N;

    std::vector<Node*> max_nodes;
    for (const auto& child : root->children) {
        if (child.second->N == max_visits) {
            max_nodes.push_back(child.second);
        }
    }

    return max_nodes[rand() % max_nodes.size()]->move;
}

void MCTS::move(int move) {
    if (root->children.count(move)) {
        root_state.move(move);
        root = root->children[move];
        return;
    }

    root_state.move(move);
    deleteNodes(root);
    root = new Node(-1, nullptr);
}

std::pair<int, int> MCTS::statistics() const {
    return std::make_pair(num_rollouts, run_time);
}

void MCTS::deleteNodes(Node* node){
        for (const auto& child : node->children) {
            deleteNodes(child.second);
        }
        delete node;
}



