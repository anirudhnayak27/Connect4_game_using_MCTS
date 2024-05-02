#include "ConnectState.hpp"
#include <iostream>
#include <algorithm>

ConnectState::ConnectState() : 
    board(GameMeta::ROWS, std::vector<int>(GameMeta::COLS, 0)),
    to_play(GameMeta::PLAYERS.at("one")),
    height(GameMeta::COLS, GameMeta::ROWS - 1),
    last_played(2, -1) {}

std::vector<std::vector<int>> ConnectState::get_board() {
    return board;
}

void ConnectState::move(int col) {
    board[height[col]][col] = to_play;
    last_played = {height[col], col};
    height[col]--;
    to_play = (to_play == GameMeta::PLAYERS.at("one")) ? GameMeta::PLAYERS.at("two"): GameMeta::PLAYERS.at("one");
}

std::vector<int> ConnectState::get_legal_moves() {
    std::vector<int> legal_moves;
    for (int col = 0; col < GameMeta::COLS; ++col) {
        if (board[0][col] == 0) {
            legal_moves.push_back(col);
        }
    }
    return legal_moves;
}

int ConnectState::check_win() {
    if (last_played.size() > 0 && check_win_from(last_played[0], last_played[1])) {
        return board[last_played[0]][last_played[1]];
    }
    return 0;
}

bool ConnectState::check_win_from(int row, int col) {
    if (row < 0 || row >= GameMeta::ROWS || col < 0 || col >= GameMeta::COLS) {
        return false;  // Return false if out of bounds
    }
    int player = board[row][col];

    // Check horizontal
    int consecutive = 1;
    int tmpcol = col + 1;
    while (tmpcol < GameMeta::COLS && board[row][tmpcol] == player) {
        consecutive++;
        tmpcol++;
    }
    tmpcol = col - 1;
    while (tmpcol >= 0 && board[row][tmpcol] == player) {
        consecutive++;
        tmpcol--;
    }
    if (consecutive >= 4) {
        return true;
    }

    // Check vertical
    consecutive = 1;
    int tmprow = row + 1;
    while (tmprow < GameMeta::ROWS && board[tmprow][col] == player) {
        consecutive++;
        tmprow++;
    }
    tmprow = row - 1;
    while (tmprow >= 0 && board[tmprow][col] == player) {
        consecutive++;
        tmprow--;
    }
    if (consecutive >= 4) {
        return true;
    }

    // Check diagonal
    consecutive = 1;
    tmprow = row + 1;
    tmpcol = col + 1;
    while (tmprow < GameMeta::ROWS && tmpcol < GameMeta::COLS && board[tmprow][tmpcol] == player) {
        consecutive++;
        tmprow++;
        tmpcol++;
    }
    tmprow = row - 1;
    tmpcol = col - 1;
    while (tmprow >= 0 && tmpcol >= 0 && board[tmprow][tmpcol] == player) {
        consecutive++;
        tmprow--;
        tmpcol--;
    }
    if (consecutive >= 4) {
        return true;
    }

    // Check anti-diagonal
    consecutive = 1;
    tmprow = row + 1;
    tmpcol = col - 1;
    while (tmprow < GameMeta::ROWS && tmpcol >= 0 && board[tmprow][tmpcol] == player) {
        consecutive++;
        tmprow++;
        tmpcol--;
    }
    tmprow = row - 1;
    tmpcol = col + 1;
    while (tmprow >= 0 && tmpcol < GameMeta::COLS && board[tmprow][tmpcol] == player) {
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
    return check_win() || get_legal_moves().empty();
}

int ConnectState::get_outcome() {
    if (get_legal_moves().empty() && check_win() == 0) {
        return GameMeta::OUTCOMES.at("draw");
    }
    return (check_win() == GameMeta::PLAYERS.at("one")) ? GameMeta::OUTCOMES.at("one") : GameMeta::OUTCOMES.at("two");
}

void ConnectState::print() {
    std::cout << "=============================" << std::endl;

    for (int row = 0; row < GameMeta::ROWS; ++row) {
        for (int col = 0; col < GameMeta::COLS; ++col) {
            std::cout << "| ";
            if (board[row][col] == 1) {
                std::cout << "X ";
            } else if (board[row][col] == 2) {
                std::cout << "O ";
            } else {
                std::cout << "  ";
            }
        }
        std::cout << "|" << std::endl;
    }

    std::cout << "=============================" << std::endl;
}
