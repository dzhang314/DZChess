#include <climits>  // for INT_MIN, INT_MAX
#include <cstdint>  // for std::uint64_t
#include <iostream>

#include "ChessPiece.hpp"
#include "ChessBoard.hpp"


                // std::cout << COLOR_NAME<color> << ' ' << TYPE_NAME<type>
                //           << " at " << square_name(src).data()
                //           << " can move to " << square_name(dst).data()
                //           << std::endl;


template <PieceColor COLOR, int DEPTH>
struct MaterialisticEvaluationVisitor {

    int accumulator;

    using result_type = int;

    static constexpr result_type visit(const ChessBoard &b) noexcept {
        return (10000 * b.piece_count<PieceColor::WHITE, PieceType::KING  >()
                + 900 * b.piece_count<PieceColor::WHITE, PieceType::QUEEN >()
                + 500 * b.piece_count<PieceColor::WHITE, PieceType::ROOK  >()
                + 300 * b.piece_count<PieceColor::WHITE, PieceType::BISHOP>()
                + 300 * b.piece_count<PieceColor::WHITE, PieceType::KNIGHT>()
                + 100 * b.piece_count<PieceColor::WHITE, PieceType::PAWN  >()
              - 10000 * b.piece_count<PieceColor::BLACK, PieceType::KING  >()
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


int main() {
    ChessBoard board{};
    std::cout << board.visit<MaterialisticEvaluationVisitor, PieceColor::WHITE, 5>() << std::endl;
}
