#include "GameState.hpp"

#include <iostream>
#include <string>

int main() {
    DZChess::GameState state;
    while (true) {
        const auto moves = state.available_moves_and_names();
        std::cout << state.board() << std::endl;
        if (moves.size() == 0) {
            if (state.in_check()) {
                std::cout << "You have been checkmated! Game over." << std::endl;
                break;
            } else {
                std::cout << "You have been stalemated! Game over." << std::endl;
                break;
            }
        } else {
            if (state.in_check()) {
                std::cout << "You are in check. You have " << moves.size()
                          << " legal moves:" << std::endl;
                for (const auto &[move, name] : moves) {
                    std::cout << "    " << name << std::endl;
                }
                std::cout << std::endl;
            }
            std::string selected_move;
            std::cin >> selected_move;
            std::cout << std::endl;
            int num_matches = 0;
            for (const auto &[move, name] : moves) {
                if (name == selected_move) { ++num_matches; }
            }
            if (num_matches == 0) {
                std::cout << selected_move << " is not a legal move. "
                          << "The legal moves in this position are:" << std::endl;
                for (const auto &[move, name] : moves) {
                    std::cout << "    " << name << std::endl;
                }
                std::cout << std::endl;
            } else if (num_matches == 1) {
                for (const auto &[move, name] : moves) {
                    if (name == selected_move) { state.make_move(move); }
                }
            } else {
                std::cout << "ERROR: Move " << selected_move << "is ambiguous."
                          << std::endl;
                break;
            }
        }
    }
    return 0;
}
