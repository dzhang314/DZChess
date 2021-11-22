#ifndef DZCHESS_MOVE_NAMING_HPP_INCLUDED
#define DZCHESS_MOVE_NAMING_HPP_INCLUDED

#include <cstdint> // for std::uint64_t
#include <sstream> // for std::ostringstream
#include <string>  // for std::string
#include <utility> // for std::pair
#include <vector>  // for std::vector

#include "ChessPiece.hpp"
#include "ChessBoard.hpp"

namespace DZChess {


struct ChessMove {
    std::uint64_t src;
    std::uint64_t dst;
    PieceType src_type;
    PieceType dst_type;
};


template <PieceColor COLOR, int DEPTH>
struct MoveListVisitor {

    using result_type = bool;

    static constexpr bool visit(const ChessBoard &) noexcept { return true; }

}; // struct MoveListVisitor


template <PieceColor COLOR>
struct MoveListVisitor<COLOR, 1> {

    std::vector<std::pair<ChessMove, ChessBoard>> &moves;

    using result_type = bool;

    explicit MoveListVisitor(
        std::vector<std::pair<ChessMove, ChessBoard>> &moves
    ) : moves(moves) {}

    constexpr result_type get_result() const noexcept { return true; }

    template <PieceType TYPE>
    void visit(const ChessBoard &, const ChessBoard &next,
               std::uint64_t src, std::uint64_t dst, bool) noexcept {
        ChessMove move{src, dst, TYPE, TYPE};
        moves.emplace_back(move, next);
    }

    template <PieceType TYPE>
    void visit_promotion(const ChessBoard &, const ChessBoard &next,
                         std::uint64_t src, std::uint64_t dst, bool) noexcept {
        ChessMove move{src, dst, PieceType::PAWN, TYPE};
        moves.emplace_back(move, next);
    }

}; // struct MoveListVisitor<COLOR, 1>


template <PieceColor COLOR>
std::vector<std::pair<std::string, ChessBoard>>
available_moves_and_names(const ChessBoard &board) {

    std::vector<std::pair<ChessMove, ChessBoard>> moves{};
    MoveListVisitor<COLOR, 1> visitor{moves};
    board.visit(visitor);

    std::vector<std::pair<std::string, ChessBoard>> result{};
    for (const auto &[move, next] : moves) {

        const bool is_capture = board.is_occupied(move.dst);
        const std::uint64_t src_rank = move.src / 8;
        const std::uint64_t src_file = move.src % 8;
        const std::uint64_t dst_rank = move.dst / 8;
        const std::uint64_t dst_file = move.dst % 8;
        std::ostringstream name{};

        switch (move.src_type) {
            case PieceType::KING  : { name << 'K'; break; }
            case PieceType::QUEEN : { name << 'Q'; break; }
            case PieceType::ROOK  : { name << 'R'; break; }
            case PieceType::BISHOP: { name << 'B'; break; }
            case PieceType::KNIGHT: { name << 'N'; break; }
            case PieceType::PAWN: {
                if (is_capture) {
                    name << static_cast<char>('a' + src_file);
                }
                break;
            }
        }

        if (move.src_type != PieceType::PAWN) {
            bool ambiguous_rank = false;
            bool ambiguous_file = false;
            bool ambiguous_diag = false;
            for (const auto &[other, onext] : moves) {
                if ((move.src_type == other.src_type) && (move.dst == other.dst)) {
                    const std::uint64_t osrc_rank = other.src / 8;
                    const std::uint64_t osrc_file = other.src % 8;
                    if ((osrc_rank == src_rank) && (osrc_file != src_file)) {
                        ambiguous_rank = true;
                    }
                    if ((osrc_rank != src_rank) && (osrc_file == src_file)) {
                        ambiguous_file = true;
                    }
                    if ((osrc_rank != src_rank) && (osrc_file != src_file)) {
                        ambiguous_diag = true;
                    }
                }
            }
            if (ambiguous_rank || ambiguous_file || ambiguous_diag) {
                if (!ambiguous_file) {
                    name << static_cast<char>('a' + src_file);
                } else if (!ambiguous_rank) {
                    name << static_cast<char>('1' + src_rank);
                } else {
                    name << static_cast<char>('a' + src_file);
                    name << static_cast<char>('1' + src_rank);
                }
            }
        }

        if (is_capture) { name << 'x'; }
        name << static_cast<char>('a' + dst_file);
        name << static_cast<char>('1' + dst_rank);

        if (move.src_type != move.dst_type) {
            name << '=';
            switch (move.dst_type) {
                case PieceType::KING  : { name << 'K'; break; }
                case PieceType::QUEEN : { name << 'Q'; break; }
                case PieceType::ROOK  : { name << 'R'; break; }
                case PieceType::BISHOP: { name << 'B'; break; }
                case PieceType::KNIGHT: { name << 'N'; break; }
                case PieceType::PAWN  : { name << 'P'; break; }
            }
        }

        result.emplace_back(name.str(), next);
    }
    return result;
}


} // namespace DZChess

#endif // DZCHESS_MOVE_NAMING_HPP_INCLUDED
