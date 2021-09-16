#include "GameState.hpp"

#include <iostream>
#include <string>


namespace DZChess::ECO {

    constexpr GameState INITIAL_STATE;
    //constexpr GameState B00_KINGS_PAWN = INITIAL_STATE.after_move_checked("e2e4");
    //constexpr GameState C20_KINGS_PAWN = B00_KINGS_PAWN.after_move_checked("e7e5");
    //constexpr GameState C40_KINGS_KNIGHT = C20_KINGS_PAWN.after_move_checked("g1f3");
    //constexpr GameState C44_KINGS_PAWN = C40_KINGS_KNIGHT.after_move_checked("b8c6");
    //constexpr GameState C60_RUY_LOPEZ = C44_KINGS_PAWN.after_move_checked("f1b5");

} // namespace DZChess::ECO


int main() {
    std::cout << std::endl;
    DZChess::GameState state = DZChess::ECO::INITIAL_STATE;
    while (true) {
        std::cout << state.board() << std::endl;
        const auto moves = state.available_moves_and_names();
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
                    std::cout << "    " << name << " ("
                              << -state.after_move(move).evaluate(4)
                              << ")" << std::endl;
                }
                std::cout << std::endl;
            } else if (num_matches == 1) {
                for (const auto &[move, name] : moves) {
                    if (name == selected_move) { state.make_move(move); }
                }
            } else {
                std::cout << "ERROR: Move " << selected_move << " is ambiguous."
                          << std::endl;
                break;
            }
        }
    }
    return 0;
}
