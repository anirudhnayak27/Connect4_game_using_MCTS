#include <iostream>
#include "ConnectState.hpp"
#include "mcts.hpp"

int main() {
    // Initialize the game state
    ConnectState state;

    // Initialize the Monte Carlo Tree Search
    MCTS mcts(state);

    // Run the game loop
    while (!state.game_over()) {
        int move;

        // Display the current state of the game
        state.print();

        // Player's turn
        std::cout << "Your turn. Enter your move (column number): ";
        std::cin >> move;

        // Make the player's move
        state.move(move);

        // Check if the game is over after the player's move
        if (state.game_over()) {
            break;
        }

        // AI's turn using Monte Carlo Tree Search
        std::cout << "AI is thinking...\n";
        mcts.search(1000); // Adjust time limit as needed
        int ai_move = mcts.best_move();

        // Make the AI's move
        state.move(ai_move);

        // Display the AI's move
        std::cout << "AI plays column " << ai_move << std::endl;
    }

    // Display the final state of the game
    state.print();

    // Determine the outcome of the game
    int outcome = state.get_outcome();

    auto draw_iter = GameMeta::OUTCOMES.find("draw");
    auto one_iter = GameMeta::OUTCOMES.find("one");
    auto two_iter = GameMeta::OUTCOMES.find("two");

    if (outcome == draw_iter->second) {
        std::cout << "The game ends in a draw!" << std::endl;
    } else if (outcome == one_iter->second) {
        std::cout << "Congratulations! You win!" << std::endl;
    } else if (outcome == two_iter->second) {
        std::cout << "AI wins! Better luck next time." << std::endl;
    }


    return 0;
}
