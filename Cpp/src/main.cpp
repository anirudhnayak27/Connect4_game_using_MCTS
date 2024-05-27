#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <limits>
#include <random>
#include <chrono>
#include <algorithm>
#include "connect_4.h"


int main() {
    std::random_device rd;
    std::mt19937 gen(rd());

    play();

    return 0;
}

void play() {
    ConnectState state;
    MCTS mcts(state);
    mcts.set_max_rollouts(100000);
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