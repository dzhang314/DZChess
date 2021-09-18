#include "GameState.hpp"
#include "ChessPlayer.hpp"

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

    PieceColor run_game(ChessPlayer *white_player, ChessPlayer *black_player, GameState state, bool verbose = false) {
        if (verbose) { std::cout << std::endl; }
        while (true) {
            if (state.color_to_move() == PieceColor::BLACK) {
                if (verbose) {
                    std::cout << "Black to move." << std::endl << std::endl;
                    std::cout << state.board() << std::endl;
                }
                if (state.available_moves().empty()) {
                    return state.in_check() ? PieceColor::WHITE : PieceColor::NONE;
                }
                state.make_move(black_player->select_move(state));
            } else {
                if (verbose) {
                    std::cout << "White to move." << std::endl << std::endl;
                    std::cout << state.board() << std::endl;
                }
                if (state.available_moves().empty()) {
                    return state.in_check() ? PieceColor::BLACK : PieceColor::NONE;
                }
                state.make_move(white_player->select_move(state));
            }
        }
    }

}


int main() {

    DZChess::ChessPlayer *white_player = new DZChess::MaterialisticPlayer{4, true};
    DZChess::ChessPlayer *black_player = new DZChess::ConsolePlayer{};
    std::cout << std::endl;
    int white_wins = 0;
    int black_wins = 0;
    int draws = 0;
    for (int i = 0; i < 100; ++i) {
        switch (run_game(white_player, black_player, DZChess::ECO::INITIAL_STATE, true)) {
            case DZChess::PieceColor::WHITE:
                ++white_wins;
                break;
            case DZChess::PieceColor::BLACK:
                ++black_wins;
                break;
            case DZChess::PieceColor::NONE:
                ++draws;
                break;
        }
        std::cout << white_wins << " : " << draws << " : " << black_wins << std::endl;
    }
    return 0;
}
