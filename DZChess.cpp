#include "GameState.hpp"
#include "MaterialisticPlayer.hpp"

#include <cstdlib> // for std::exit
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


namespace DZChess {

    void check_for_end_of_game(const GameState &state) {
        const auto moves = state.available_moves();
        if (moves.size() == 0) {
            if (state.in_check()) {
                std::cout << "You have been checkmated! Game over." << std::endl;
                std::exit(EXIT_SUCCESS);
            } else {
                std::cout << "You have been stalemated! Game over." << std::endl;
                std::exit(EXIT_SUCCESS);
            }
        }
    }

    ChessMove get_move_from_player(const GameState &state) {
        const auto moves = state.available_moves_and_names();
        if (state.in_check()) {
            std::cout << "You are in check. You have " << moves.size()
                      << " legal moves:" << std::endl;
            for (const auto &[move, name] : moves) {
                std::cout << "    " << name << std::endl;
            }
            std::cout << std::endl;
        }
        while (true) {
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
                    if (name == selected_move) { return move; }
                }
            } else {
                std::cout << "ERROR: Move " << selected_move << " is ambiguous."
                          << std::endl;
                std::exit(EXIT_SUCCESS);
            }
        }
    }

}


int main() {

    DZChess::GameState state = DZChess::ECO::INITIAL_STATE;
    DZChess::MaterialisticPlayer player;
    std::cout << std::endl;

    while (true) {
        std::cout << state.board() << std::endl;
        DZChess::check_for_end_of_game(state);
        state.make_move(DZChess::get_move_from_player(state));
        std::cout << state.board() << std::endl;
        DZChess::check_for_end_of_game(state);
        state.make_move(player.select_move(state));
    }

    return 0;
}
