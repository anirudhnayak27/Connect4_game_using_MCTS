
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <limits>
#include <random>
#include <chrono>
#include <algorithm>

#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/reduce.h>


class GameMeta {
public:
    static const std::unordered_map<std::string, int> PLAYERS;
    static const std::unordered_map<std::string, int> OUTCOMES;
    static constexpr double INF = std::numeric_limits<double>::infinity();
    static const int ROWS = 6;
    static const int COLS = 7;
};

const std::unordered_map<std::string, int> GameMeta::PLAYERS = {
    {"none", 0}, {"one", 1}, {"two", 2}
};

const std::unordered_map<std::string, int> GameMeta::OUTCOMES = {
    {"none", 0}, {"one", 1}, {"two", 2}, {"draw", 3}
};

class MCTSMeta {
public:
    static const double EXPLORATION;
};

const double MCTSMeta::EXPLORATION = std::sqrt(2.0);

class ConnectState {
public:
    ConnectState() {
        board = std::vector<std::vector<int>>(GameMeta::ROWS, std::vector<int>(GameMeta::COLS, 0));
        to_play = GameMeta::PLAYERS.at("one");
        height = std::vector<int>(GameMeta::COLS, GameMeta::ROWS - 1);
    }

    std::vector<std::vector<int>> get_board() const {
        return board;
    }

    void move(int col) {
        board[height[col]][col] = to_play;
        last_played = {height[col], col};
        height[col]--;
        to_play = (to_play == GameMeta::PLAYERS.at("one")) ? GameMeta::PLAYERS.at("two") : GameMeta::PLAYERS.at("one");
    }

    std::vector<int> get_legal_moves() const {
        std::vector<int> legal_moves;
        for (int col = 0; col < GameMeta::COLS; ++col) {
            if (board[0][col] == 0) {
                legal_moves.push_back(col);
            }
        }
        return legal_moves;
    }

    int check_win() {
        if (!last_played.empty() && check_win_from(last_played[0], last_played[1])) {
            return board[last_played[0]][last_played[1]];
        }
        return 0;
    }

    bool check_win_from(int row, int col) {
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

    bool game_over()  {
        return check_win() != 0 || get_legal_moves().empty();
    }

    int get_outcome()  {
        if (get_legal_moves().empty() && check_win() == 0) {
            return GameMeta::OUTCOMES.at("draw");
        }
        return (check_win() == GameMeta::PLAYERS.at("one")) ? GameMeta::OUTCOMES.at("one") : GameMeta::OUTCOMES.at("two");
    }

    void print() const {
        std::cout << "=============================\n";
        for (int row = 0; row < GameMeta::ROWS; ++row) {
            for (int col = 0; col < GameMeta::COLS; ++col) {
                std::cout << "| " << ((board[row][col] == 1) ? 'X' : (board[row][col] == 2) ? 'O' : ' ') << " ";
            }
            std::cout << "|\n";
        }
        std::cout << "=============================\n";
    }

private:
    std::vector<std::vector<int>> board;
    int to_play;
    std::vector<int> height;
    std::vector<int> last_played;
};

class Node {
public:
    Node(int move, Node* parent)
        : move(move), parent(parent), N(0), Q(0), outcome(GameMeta::PLAYERS.at("none")) {}

    void add_children(const std::vector<Node*>& children) {
        for (Node* child : children) {
            this->children[child->move] = child;
        }
    }

    double value(double explore = MCTSMeta::EXPLORATION) const {
        if (N == 0) {
            return (explore == 0) ? 0 : GameMeta::INF;
        } else {
            return Q / static_cast<double>(N) + explore * std::sqrt(std::log(static_cast<double>(parent->N)) / static_cast<double>(N));
        }
    }

    int move;
    Node* parent;
    int N;
    double Q;
    std::unordered_map<int, Node*> children;
    int outcome;
};



// CUDA error checking macro
#define cudaCheckError(ans) { cudaAssert((ans), __FILE__, __LINE__); }
inline void cudaAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess)
   {
      fprintf(stderr,"CUDA Error: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

// CUDA kernel to find the index of the maximum value (same as before)
__global__ void find_max_index_kernel(double* values, int num_values, int* max_idx) {
    extern __shared__ char shared_mem[];
    double* shared_max = reinterpret_cast<double*>(shared_mem);
    int* shared_idx = reinterpret_cast<int*>(shared_max + blockDim.x);

    int tid = threadIdx.x;
    int gid = blockIdx.x * blockDim.x + threadIdx.x;

    if (gid < num_values) {
        shared_max[tid] = values[gid];
        shared_idx[tid] = gid;
    } else {
        shared_max[tid] = -GameMeta::INF;
        shared_idx[tid] = -1;
    }

    __syncthreads();

    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s && gid < num_values) {
            if (shared_max[tid] < shared_max[tid + s]) {
                shared_max[tid] = shared_max[tid + s];
                shared_idx[tid] = shared_idx[tid + s];
            }
        }
        __syncthreads();
    }

    if (tid == 0) {
        *max_idx = shared_idx[0];
    }
}

/*__global__ void find_max_index_kernel(double* values, int num_values, int* max_idx) {
    *max_idx = 0;  // Just return the first index for now
}*/

// Modified function to find max UCT value using CUDA
int find_max_uct_child(const std::unordered_map<int, Node*>& children, Node* parent, double explore = MCTSMeta::EXPLORATION) {
    int num_children = children.size();
    if (num_children == 0) return -1;

    std::vector<int> moves;
    std::vector<double> uct_values;

    for (const auto& pair : children) {
        Node* child = pair.second;
        moves.push_back(pair.first);

        double uct_value;
        if (child == nullptr) {
            uct_value = -GameMeta::INF;
        } else if (child->N == 0) {
            uct_value = (explore == 0) ? 0 : GameMeta::INF;
        } else {
            uct_value = child->Q / static_cast<double>(child->N) +
                       explore * std::sqrt(std::log(static_cast<double>(parent->N)) / static_cast<double>(child->N));
        }
        uct_values.push_back(uct_value);
    }

    // Use managed memory for uct_values
    double* d_uct_values;
    cudaCheckError(cudaMallocManaged(&d_uct_values, num_children * sizeof(double)));

    // Copy host data to managed memory
    for (int i = 0; i < num_children; ++i) {
        d_uct_values[i] = uct_values[i];
    }

    // Find max index using our custom kernel
    int* d_max_idx;
    cudaCheckError(cudaMallocManaged(&d_max_idx, sizeof(int)));
    *d_max_idx = -1;  // Initialize on host

    int num_threads = 128;
    size_t shared_mem_size = num_threads * (sizeof(double) + sizeof(int));

    find_max_index_kernel<<<1, num_threads, shared_mem_size>>>(
        d_uct_values,
        num_children,
        d_max_idx
    );
    cudaCheckError(cudaGetLastError());
    cudaCheckError(cudaDeviceSynchronize());

    int max_idx = *d_max_idx;  // Copy from managed memory

    // Free managed memory
    cudaCheckError(cudaFree(d_uct_values));
    cudaCheckError(cudaFree(d_max_idx));

    return moves[max_idx];
}






class MCTS {
public:
    MCTS(const ConnectState& state) : root_state(state) {
        root = new Node(-1, nullptr);
    }

    ~MCTS() {
        deleteNodes(root);
    }

    void set_max_rollouts(int max_rollouts) {
        this->max_rollouts = max_rollouts;
    }


    // Modified select_node function (same as before)
    std::pair<Node*, ConnectState> select_node() {
        Node* node = root;
        ConnectState state = root_state;
        int depth = 0;

        while (!node->children.empty() && depth < GameMeta::ROWS * GameMeta::COLS) {
           // std::cout << "Selecting at depth " << depth << ", children: " << node->children.size() << "\n";
            int max_move = find_max_uct_child(node->children, node);
            //std::cout << "Selected move: " << max_move << "\n";
            node = node->children[max_move];
            state.move(node->move);
            depth++;

            if (node->N == 0) {
               // std::cout << "Returning unexplored node at depth " << depth << "\n";
                return std::make_pair(node, state);
            }
        }

        if (expand(node, state)) {
            node = node->children.begin()->second;
            state.move(node->move);
        }

        return std::make_pair(node, state);
    }




    bool expand(Node* parent, ConnectState& state) {
        if (state.game_over()) {
            return false;
        }

        std::vector<Node*> children;
        for (int move : state.get_legal_moves()) {
          //std::cout << "Adding child with move: " << move << "\n";
          children.push_back(new Node(move, parent));
        }
        parent->add_children(children);

        return true;
    }

    int roll_out(const ConnectState& state) {
        ConnectState rollout_state = state;
        while (!rollout_state.game_over()) {
            std::vector<int> legal_moves = rollout_state.get_legal_moves();
            rollout_state.move(legal_moves[rand() % legal_moves.size()]);
        }
        return rollout_state.get_outcome();
    }

    void back_propagate(Node* node, int turn, int outcome) {
        int reward = (outcome == turn) ? 0 : 1;
        while (node != nullptr) {
            /*std::cout << "Updating node (move=" << node->move << "): N=" << node->N << " -> " << node->N+1
                      << ", Q=" << node->Q << " -> " << node->Q+reward << "\n";*/
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

    void search() {
        auto start_time = std::chrono::steady_clock::now();
        num_rollouts = 0;
        while (!root_state.game_over() && num_rollouts < max_rollouts) {
            //std::cout << "Starting rollout " << num_rollouts << "\n";
            Node* node;
            ConnectState state;
            std::tie(node, state) = select_node();
            int outcome = roll_out(state);
            //std::cout << "Rollout outcome: " << outcome << "\n";
            back_propagate(node, state.get_legal_moves().empty() ? GameMeta::PLAYERS.at("none") : state.get_legal_moves()[0], outcome);
            num_rollouts++;
            if (num_rollouts >= max_rollouts) break;
        }

        run_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
    }

    int best_move()  {

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

    void move(int move) {
        if (root->children.count(move)) {
            root_state.move(move);
            root = root->children[move];
            return;
        }

        root_state.move(move);
        deleteNodes(root);
        root = new Node(-1, nullptr);
    }

    std::pair<int, int> statistics() const {
        return std::make_pair(num_rollouts, run_time);
    }

private:
    void deleteNodes(Node* node) {
        for (const auto& child : node->children) {
            deleteNodes(child.second);
        }
        delete node;
    }

    ConnectState root_state;
    Node* root;
    int num_rollouts;
    int run_time;
    int max_rollouts = 1;
};

void play() {
    ConnectState state;
    MCTS mcts(state);
    mcts.set_max_rollouts(1000);
    std::string quit = "quit";

    while (!state.game_over()) {
        std::cout << "Current state:\n";
        state.print();

        std::string user_move;
        std::vector<int> legal_moves = state.get_legal_moves();
        do {
            std::cout << "Enter a move (or 'quit' to end the game): ";
            std::cin >> user_move;
        } while (user_move != quit && std::find(legal_moves.begin(), legal_moves.end(), std::stoi(user_move)) == legal_moves.end());

        if (user_move == quit) {
            std::cout << "Quitting...\n";
            std::cout << "Player two won!\n";
            state.print();
            break;
        }

        state.move(std::stoi(user_move));
        mcts.move(std::stoi(user_move));
        // state.print();

        if (state.game_over()) {
            std::cout << "Player one won!\n";
            state.print();
            break;
        }

        std::cout << "Thinking...\n";

        mcts.search();
        int num_rollouts, run_time;
        std::tie(num_rollouts, run_time) = mcts.statistics();
        std::cout << "Statistics: " << num_rollouts << " rollouts in " << mcts.statistics().second << " milliseconds\n";
        int move = mcts.best_move();

        std::cout << "MCTS chose move: " << move << "\n";

        state.move(move);
        mcts.move(move);

        if (state.game_over()) {
            std::cout << "Player two won!\n";
            state.print();
            break;
        }
    }
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());

    play();

    return 0;
}
