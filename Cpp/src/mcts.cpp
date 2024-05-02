#include "mcts.hpp"
#include <algorithm>

MCTS::MCTS(const ConnectState& state) : root_state(state), root(new Node(-1, nullptr)), run_time(0), node_count(0), num_rollouts(0) {}

Node* MCTS::select_node() {
    Node* node = root;
    ConnectState state = root_state;

    while (!node->children.empty()) {
        std::vector<Node*> children;
        for (auto& [key, value] : node->children) {
            children.push_back(value);
        }
        double max_value = (*std::max_element(children.begin(), children.end(), [](Node* a, Node* b) { return a->value() < b->value(); }))->value();
        std::vector<Node*> max_nodes;
        for (auto& child : children) {
            if (child->value() == max_value) {
                max_nodes.push_back(child);
            }
        }
        node = max_nodes[rand() % max_nodes.size()];
        state.move(node->move);
        if (node->N == 0) {
            return node;
        }
    }

    if (expand(node, state)) {
        node = node->children.begin()->second;
        state.move(node->move);
    }

    return node;
}

bool MCTS::expand(Node* parent, ConnectState& state) {
    if (state.game_over()) {
        return false;
    }

    std::vector<int> legal_moves = state.get_legal_moves();
    std::unordered_map<int, Node*> children;
    for (int move : legal_moves) {
        children[move] = new Node(move, parent);
    }
    parent->add_children(children);

    return true;
}

int MCTS::roll_out(ConnectState& state) {
    while (!state.game_over()) {
        state.move(state.get_legal_moves()[rand() % state.get_legal_moves().size()]);
    }

    return state.get_outcome();
}

void MCTS::back_propagate(Node* node, int turn, int outcome) {
    int reward = (outcome == turn) ? 0 : 1;

    while (node != nullptr) {
        node->N++;
        node->Q += reward;
        node = node->parent;
        reward = (outcome == GameMeta::OUTCOMES.at("draw")) ? 0 : 1 - reward;
    }
}

void MCTS::search(int time_limit) {
    auto start_time = std::chrono::steady_clock::now();

    num_rollouts = 0;
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() < time_limit) {
        Node* node = select_node();
        ConnectState state = root_state;
        int outcome = roll_out(state);
        back_propagate(node, state.get_to_play(), outcome); // Use getter function
        num_rollouts++;
    }

    auto end_time = std::chrono::steady_clock::now();
    run_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.0;
}


int MCTS::best_move() {
    if (root_state.game_over()) {
        return -1;
    }

    double max_value = std::max_element(root->children.begin(), root->children.end(), [](const std::pair<int, Node*>& a, const std::pair<int, Node*>& b) { return a.second->N < b.second->N; })->second->N;
    std::vector<Node*> max_nodes;
    for (auto& [key, value] : root->children) {
        if (value->N == max_value) {
            max_nodes.push_back(value);
        }
    }
    Node* best_child = max_nodes[rand() % max_nodes.size()];

    return best_child->move;
}

void MCTS::move(int move) {
    if (root->children.find(move) != root->children.end()) {
        root_state.move(move);
        root = root->children[move];
        return;
    }

    root_state.move(move);
    root = new Node(-1, nullptr);
}

std::pair<int, double> MCTS::statistics() {
    return {num_rollouts, run_time};
}
