#include "GameState.hpp"
#include <iostream>

int main() {
    DZChess::GameState state;
    while (true) {
        std::cout << state.board() << std::endl;
        const auto moves_and_names = state.available_moves_and_names();
        int i = 0;
        for (const auto &[move, name] : moves_and_names) {
            std::cout << '(' << i++ << ") : " << name << std::endl;
        }
        std::cout << std::endl;
        std::cin >> i;
        std::cout << std::endl;
        state.make_move(moves_and_names[i].first);
    }
}
