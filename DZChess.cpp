#include "GameState.hpp"
#include <iostream>

int main() {
    DZChess::GameState state;
    while (true) {
        std::cout << state.board() << std::endl;
        const auto moves = state.available_moves();
        int i = 0;
        for (const auto &move : moves) {
            std::cout << '(' << i++ << ") : " << move << std::endl;
        }
        std::cout << std::endl;
        std::cin >> i;
        state.make_move(moves[i]);
    }
}
