#include <climits>  // for INT_MIN, INT_MAX
#include <cstdint>  // for std::uint64_t
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ChessPiece.hpp"
#include "ChessBoard.hpp"
#include "MoveNaming.hpp"


using DZChess::PieceColor, DZChess::PieceType, DZChess::ChessBoard;


template <PieceColor COLOR, int DEPTH>
struct MaterialisticEvaluationVisitor {

    int accumulator;

    using result_type = int;

    static constexpr result_type visit(const ChessBoard &b) noexcept {
        if (b.piece_count<PieceColor::WHITE, PieceType::KING>() == 0)
            return -1'000'000;
        if (b.piece_count<PieceColor::BLACK, PieceType::KING>() == 0)
            return +1'000'000;
        return (+ 900 * b.piece_count<PieceColor::WHITE, PieceType::QUEEN >()
                + 500 * b.piece_count<PieceColor::WHITE, PieceType::ROOK  >()
                + 300 * b.piece_count<PieceColor::WHITE, PieceType::BISHOP>()
                + 300 * b.piece_count<PieceColor::WHITE, PieceType::KNIGHT>()
                + 100 * b.piece_count<PieceColor::WHITE, PieceType::PAWN  >()
                - 900 * b.piece_count<PieceColor::BLACK, PieceType::QUEEN >()
                - 500 * b.piece_count<PieceColor::BLACK, PieceType::ROOK  >()
                - 300 * b.piece_count<PieceColor::BLACK, PieceType::BISHOP>()
                - 300 * b.piece_count<PieceColor::BLACK, PieceType::KNIGHT>()
                - 100 * b.piece_count<PieceColor::BLACK, PieceType::PAWN  >());
    }

    explicit constexpr MaterialisticEvaluationVisitor() noexcept
        : accumulator((COLOR == PieceColor::WHITE) ? INT_MIN : INT_MAX) {}

    template <PieceType TYPE>
    constexpr void visit(const ChessBoard &, const ChessBoard &,
                         std::uint64_t, std::uint64_t,
                         result_type result) noexcept {
        if constexpr (COLOR == PieceColor::WHITE) {
            accumulator = std::max(accumulator, result);
        } else if constexpr (COLOR == PieceColor::BLACK) {
            accumulator = std::min(accumulator, result);
        }
    }

    template <PieceType TYPE>
    constexpr void visit_promotion(const ChessBoard &, const ChessBoard &,
                                   std::uint64_t, std::uint64_t,
                                   result_type result) noexcept {
        if constexpr (COLOR == PieceColor::WHITE) {
            accumulator = std::max(accumulator, result);
        } else if constexpr (COLOR == PieceColor::BLACK) {
            accumulator = std::min(accumulator, result);
        }
    }

    constexpr result_type get_result() const noexcept {
        return accumulator;
    }

}; // struct MaterialisticEvaluationVisitor


void print_board(const ChessBoard &board) {
    using enum PieceColor;
    using enum PieceType;
    std::cout << "    +-------------------------+" << std::endl;
    for (std::uint64_t rank = 7; rank < 8; --rank) {
        std::cout << "    | ";
        for (std::uint64_t file = 0; file < 8; ++file) {
            const std::uint64_t square = (rank << 3) | file;
            if (!board.is_occupied(square))             std::cout << "[]";
            if (board.has_piece<WHITE, KING  >(square)) std::cout << "wK";
            if (board.has_piece<WHITE, QUEEN >(square)) std::cout << "wQ";
            if (board.has_piece<WHITE, ROOK  >(square)) std::cout << "wR";
            if (board.has_piece<WHITE, BISHOP>(square)) std::cout << "wB";
            if (board.has_piece<WHITE, KNIGHT>(square)) std::cout << "wN";
            if (board.has_piece<WHITE, PAWN  >(square)) std::cout << "wP";
            if (board.has_piece<BLACK, KING  >(square)) std::cout << "bK";
            if (board.has_piece<BLACK, QUEEN >(square)) std::cout << "bQ";
            if (board.has_piece<BLACK, ROOK  >(square)) std::cout << "bR";
            if (board.has_piece<BLACK, BISHOP>(square)) std::cout << "bB";
            if (board.has_piece<BLACK, KNIGHT>(square)) std::cout << "bN";
            if (board.has_piece<BLACK, PAWN  >(square)) std::cout << "bP";
            std::cout << ' ';
        }
        std::cout << '|' << std::endl;
    }
    std::cout << "    +-------------------------+" << std::endl;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> result{};
    std::stringstream ss{s};
    std::string item;
    while (std::getline(ss, item, delim)) { result.push_back(item); }
    return result;
}


void handle_add_command(ChessBoard &b,
                        const std::vector<std::string> &tok) {
    using enum PieceColor;
    using enum PieceType;
    if (tok.size() == 4 && tok[3].size() == 2) {
        const std::uint64_t square =
            static_cast<std::uint64_t>(tok[3][0] - 'a') |
            (static_cast<std::uint64_t>(tok[3][1] - '1') << 3);
        if (tok[1] == "white") {
            if      (tok[2] == "king"  ) b.add_piece<WHITE, KING  >(square);
            else if (tok[2] == "queen" ) b.add_piece<WHITE, QUEEN >(square);
            else if (tok[2] == "rook"  ) b.add_piece<WHITE, ROOK  >(square);
            else if (tok[2] == "bishop") b.add_piece<WHITE, BISHOP>(square);
            else if (tok[2] == "knight") b.add_piece<WHITE, KNIGHT>(square);
            else if (tok[2] == "pawn"  ) b.add_piece<WHITE, PAWN  >(square);
            else std::cout << "invalid syntax for add command" << std::endl;
        } else if (tok[1] == "black") {
            if      (tok[2] == "king"  ) b.add_piece<BLACK, KING  >(square);
            else if (tok[2] == "queen" ) b.add_piece<BLACK, QUEEN >(square);
            else if (tok[2] == "rook"  ) b.add_piece<BLACK, ROOK  >(square);
            else if (tok[2] == "bishop") b.add_piece<BLACK, BISHOP>(square);
            else if (tok[2] == "knight") b.add_piece<BLACK, KNIGHT>(square);
            else if (tok[2] == "pawn"  ) b.add_piece<BLACK, PAWN  >(square);
            else std::cout << "invalid syntax for add command" << std::endl;
        } else {
            std::cout << "invalid syntax for add command" << std::endl;
        }
    } else {
        std::cout << "invalid syntax for add command" << std::endl;
    }
}


void handle_rm_command(ChessBoard &board,
                       const std::vector<std::string> &tokens) {
    if (tokens.size() == 2 && tokens[1].size() == 2) {
        const std::uint64_t square =
            static_cast<std::uint64_t>(tokens[1][0] - 'a') |
            (static_cast<std::uint64_t>(tokens[1][1] - '1') << 3);
        board.clear_square(square);
    } else {
        std::cout << "invalid syntax for rm command" << std::endl;
    }
}


using DZChess::available_moves_and_names;


void handle_ls_command(ChessBoard &board,
                       const std::vector<std::string> &tokens) {
    if (tokens.size() == 2) {
        if (tokens[1] == "white") {
            for (const auto &[name, next] :
                 available_moves_and_names<PieceColor::WHITE>(board)) {
                std::cout << name << ", ";
            }
            std::cout << std::endl;
        } else if (tokens[1] == "black") {
            for (const auto &[name, next] :
                 available_moves_and_names<PieceColor::BLACK>(board)) {
                std::cout << name << ", ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "invalid syntax for ls command" << std::endl;
        }
    } else {
        std::cout << "invalid syntax for ls command" << std::endl;
    }
}


void handle_move_command(ChessBoard &board,
                         const std::vector<std::string> &tokens) {
    if (tokens.size() == 3) {
        if (tokens[1] == "white") {
            bool found = false;
            for (const auto &[name, next] :
                 available_moves_and_names<PieceColor::WHITE>(board)) {
                if (name == tokens[2]) {
                    board = next;
                    if (found) {
                        std::cout << "ERROR: found two moves with same name"
                                  << std::endl;
                    }
                    found = true;
                }
            }
            if (!found) {
                std::cout << "ERROR: move not found for white" << std::endl;
            }
        } else if (tokens[1] == "black") {
            bool found = false;
            for (const auto &[name, next] :
                 available_moves_and_names<PieceColor::BLACK>(board)) {
                if (name == tokens[2]) {
                    board = next;
                    if (found) {
                        std::cout << "ERROR: found two moves with same name"
                                  << std::endl;
                    }
                    found = true;
                }
            }
            if (!found) {
                std::cout << "ERROR: move not found for black" << std::endl;
            }
        } else {
            std::cout << "invalid syntax for move command" << std::endl;
        }
    } else {
        std::cout << "invalid syntax for move command" << std::endl;
    }
}


void handle_eval_command(ChessBoard &board,
                         const std::vector<std::string> &tokens) {
    if (tokens.size() == 2) {

        std::vector<std::string> best_moves{};
        int best_score = 0;

        if (tokens[1] == "white") {
            for (const auto &[name, next] :
                 available_moves_and_names<PieceColor::WHITE>(board)) {
                const int score = next.visit<MaterialisticEvaluationVisitor,
                                             PieceColor::BLACK, 5>();
                std::cout << name << " : " << score << std::endl;
                if (best_moves.empty()) {
                    best_moves.push_back(name);
                    best_score = score;
                } else if (score == best_score) {
                    best_moves.push_back(name);
                } else if (score > best_score) {
                    best_moves.clear();
                    best_moves.push_back(name);
                    best_score = score;
                }
            }
            std::cout << std::endl;
            std::cout << "Best moves: ";
            for (const auto &name : best_moves) {
                std::cout << name << ", ";
            }
            std::cout << std::endl;
        } else if (tokens[1] == "black") {
            for (const auto &[name, next] :
                 available_moves_and_names<PieceColor::BLACK>(board)) {
                const int score = next.visit<MaterialisticEvaluationVisitor,
                                             PieceColor::WHITE, 5>();
                std::cout << name << " : " << score << std::endl;
                if (best_moves.empty()) {
                    best_moves.push_back(name);
                    best_score = score;
                } else if (score == best_score) {
                    best_moves.push_back(name);
                } else if (score < best_score) {
                    best_moves.clear();
                    best_moves.push_back(name);
                    best_score = score;
                }
            }
            std::cout << std::endl;
            std::cout << "Best moves: ";
            for (const auto &name : best_moves) {
                std::cout << name << ", ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "invalid syntax for eval command" << std::endl;
        }
    } else {
        std::cout << "invalid syntax for eval command" << std::endl;
    }
}


int main() {

    ChessBoard board{};

    while (true) {

        print_board(board);
        std::cout << "> ";
        std::string command;
        std::getline(std::cin, command, '\n');

        const auto tokens = split(command, ' ');
        if (!tokens.empty()) {
            if (tokens[0] == "add") {
                handle_add_command(board, tokens);
            } else if (tokens[0] == "rm") {
                handle_rm_command(board, tokens);
            } else if (tokens[0] == "ls") {
                handle_ls_command(board, tokens);
            } else if (tokens[0] == "move") {
                handle_move_command(board, tokens);
            } else if (tokens[0] == "eval") {
                handle_eval_command(board, tokens);
            } else {
                std::cout << "unknown command" << std::endl;
            }
        }

    }

}
