#ifndef DZCHESS_CHESS_BOARD_HPP_INCLUDED
#define DZCHESS_CHESS_BOARD_HPP_INCLUDED

#include <cstdint> // for std::uint64_t, UINT64_C

#include "ChessPiece.hpp"
#include "BitBoard.hpp"

namespace DZChess {


class ChessBoard {

    BitBoard white_king;
    BitBoard white_queen;
    BitBoard white_rook;
    BitBoard white_bishop;
    BitBoard white_knight;
    BitBoard white_pawn;

    BitBoard black_king;
    BitBoard black_queen;
    BitBoard black_rook;
    BitBoard black_bishop;
    BitBoard black_knight;
    BitBoard black_pawn;

    BitBoard white_pieces;
    BitBoard black_pieces;
    BitBoard all_pieces;

public:

    explicit constexpr ChessBoard(
        BitBoard wk, BitBoard wq, BitBoard wr,
        BitBoard wb, BitBoard wn, BitBoard wp,
        BitBoard bk, BitBoard bq, BitBoard br,
        BitBoard bb, BitBoard bn, BitBoard bp
    ) noexcept :
        white_king(wk), white_queen(wq), white_rook(wr),
        white_bishop(wb), white_knight(wn), white_pawn(wp),
        black_king(bk), black_queen(bq), black_rook(br),
        black_bishop(bb), black_knight(bn), black_pawn(bp),
        white_pieces(wk | wq | wr | wb | wn | wp),
        black_pieces(bk | bq | br | bb | bn | bp),
        all_pieces(white_pieces | black_pieces) {}

    explicit constexpr ChessBoard() noexcept : ChessBoard(
        UINT64_C(0x0000000000000010), UINT64_C(0x0000000000000008),
        UINT64_C(0x0000000000000081), UINT64_C(0x0000000000000024),
        UINT64_C(0x0000000000000042), UINT64_C(0x000000000000FF00),
        UINT64_C(0x1000000000000000), UINT64_C(0x0800000000000000),
        UINT64_C(0x8100000000000000), UINT64_C(0x2400000000000000),
        UINT64_C(0x4200000000000000), UINT64_C(0x00FF000000000000)
    ) {}

    constexpr bool has_piece(std::uint64_t square) const noexcept {
        return all_pieces.is_set(square);
    }

    constexpr void clear_square(std::uint64_t square) noexcept {
        const BitBoard mask{~(UINT64_C(1) << square)};
        white_king &= mask;
        white_queen &= mask;
        white_rook &= mask;
        white_bishop &= mask;
        white_knight &= mask;
        white_pawn &= mask;
        black_king &= mask;
        black_queen &= mask;
        black_rook &= mask;
        black_bishop &= mask;
        black_knight &= mask;
        black_pawn &= mask;
        white_pieces &= mask;
        black_pieces &= mask;
        all_pieces &= mask;
    }

    template <PieceColor COLOR, PieceType TYPE>
    constexpr BitBoard get_piece() const noexcept {
        if constexpr (COLOR == PieceColor::WHITE) {
            if constexpr (TYPE == PieceType::KING) {
                return white_king;
            } else if constexpr (TYPE == PieceType::QUEEN) {
                return white_queen;
            } else if constexpr (TYPE == PieceType::ROOK) {
                return white_rook;
            } else if constexpr (TYPE == PieceType::BISHOP) {
                return white_bishop;
            } else if constexpr (TYPE == PieceType::KNIGHT) {
                return white_knight;
            } else if constexpr (TYPE == PieceType::PAWN) {
                return white_pawn;
            }
        } else if constexpr (COLOR == PieceColor::BLACK) {
            if constexpr (TYPE == PieceType::KING) {
                return black_king;
            } else if constexpr (TYPE == PieceType::QUEEN) {
                return black_queen;
            } else if constexpr (TYPE == PieceType::ROOK) {
                return black_rook;
            } else if constexpr (TYPE == PieceType::BISHOP) {
                return black_bishop;
            } else if constexpr (TYPE == PieceType::KNIGHT) {
                return black_knight;
            } else if constexpr (TYPE == PieceType::PAWN) {
                return black_pawn;
            }
        }
    }

    template <PieceColor COLOR>
    constexpr BitBoard get_pieces() const noexcept {
        if constexpr (COLOR == PieceColor::WHITE) {
            return white_pieces;
        } else if constexpr (COLOR == PieceColor::BLACK) {
            return black_pieces;
        }
    }

    template <PieceColor COLOR, PieceType TYPE>
    constexpr void add_piece(std::uint64_t square) noexcept {
        const BitBoard piece{UINT64_C(1) << square};
        all_pieces |= piece;
        if constexpr (COLOR == PieceColor::WHITE) {
            white_pieces |= piece;
            if constexpr (TYPE == PieceType::KING) {
                white_king |= piece;
            } else if constexpr (TYPE == PieceType::QUEEN) {
                white_queen |= piece;
            } else if constexpr (TYPE == PieceType::ROOK) {
                white_rook |= piece;
            } else if constexpr (TYPE == PieceType::BISHOP) {
                white_bishop |= piece;
            } else if constexpr (TYPE == PieceType::KNIGHT) {
                white_knight |= piece;
            } else if constexpr (TYPE == PieceType::PAWN) {
                white_pawn |= piece;
            }
        } else if constexpr (COLOR == PieceColor::BLACK) {
            black_pieces |= piece;
            if constexpr (TYPE == PieceType::KING) {
                black_king |= piece;
            } else if constexpr (TYPE == PieceType::QUEEN) {
                black_queen |= piece;
            } else if constexpr (TYPE == PieceType::ROOK) {
                black_rook |= piece;
            } else if constexpr (TYPE == PieceType::BISHOP) {
                black_bishop |= piece;
            } else if constexpr (TYPE == PieceType::KNIGHT) {
                black_knight |= piece;
            } else if constexpr (TYPE == PieceType::PAWN) {
                black_pawn |= piece;
            }
        }
    }

    template <PieceColor COLOR, PieceType TYPE>
    constexpr int piece_count() const noexcept {
        return get_piece<COLOR, TYPE>().popcount();
    }

    template <template <PieceColor, int> typename Visitor,
              PieceColor COLOR, int DEPTH, PieceType TYPE>
    constexpr void visit_piece_moves(
        Visitor<COLOR, DEPTH> &visitor
    ) const noexcept {
        for (const std::uint64_t src : get_piece<COLOR, TYPE>()) {
            const BitBoard destinations =
                all_pieces.moves<COLOR, TYPE>(src, get_pieces<COLOR>());
            for (const std::uint64_t dst : destinations) {
                ChessBoard next = *this;
                next.clear_square(src);
                next.clear_square(dst);
                next.add_piece<COLOR, TYPE>(dst);
                visitor.template visit<TYPE>(
                    *this, next, src, dst,
                    next.visit<Visitor, other(COLOR), DEPTH - 1>()
                );
            }
        }
    }

    template <template <PieceColor, int> typename Visitor,
              PieceColor COLOR, int DEPTH>
    constexpr void visit_pawn_moves(
        Visitor<COLOR, DEPTH> &visitor
    ) const noexcept {
        for (const std::uint64_t src : get_piece<COLOR, PieceType::PAWN>()) {
            const BitBoard destinations = all_pieces.moves<
                COLOR, PieceType::PAWN
            >(src, get_pieces<COLOR>());
            if constexpr (COLOR == PieceColor::WHITE) {
                if ((48 <= src) && (src < 56)) {
                    for (const std::uint64_t dst : destinations) {
                        ChessBoard next = *this;
                        next.clear_square(src);
                        next.clear_square(dst);
                        next.add_piece<COLOR, PieceType::QUEEN>(dst);
                        visitor.template visit_promotion<PieceType::QUEEN>(
                            *this, next, src, dst,
                            next.visit<Visitor, other(COLOR), DEPTH - 1>()
                        );
                        next.clear_square(dst);
                        next.add_piece<COLOR, PieceType::ROOK>(dst);
                        visitor.template visit_promotion<PieceType::ROOK>(
                            *this, next, src, dst,
                            next.visit<Visitor, other(COLOR), DEPTH - 1>()
                        );
                        next.clear_square(dst);
                        next.add_piece<COLOR, PieceType::BISHOP>(dst);
                        visitor.template visit_promotion<PieceType::BISHOP>(
                            *this, next, src, dst,
                            next.visit<Visitor, other(COLOR), DEPTH - 1>()
                        );
                        next.clear_square(dst);
                        next.add_piece<COLOR, PieceType::KNIGHT>(dst);
                        visitor.template visit_promotion<PieceType::KNIGHT>(
                            *this, next, src, dst,
                            next.visit<Visitor, other(COLOR), DEPTH - 1>()
                        );
                    }
                } else {
                    for (const std::uint64_t dst : destinations) {
                        ChessBoard next = *this;
                        next.clear_square(src);
                        next.clear_square(dst);
                        next.add_piece<COLOR, PieceType::PAWN>(dst);
                        visitor.template visit<PieceType::PAWN>(
                            *this, next, src, dst,
                            next.visit<Visitor, other(COLOR), DEPTH - 1>()
                        );
                    }
                }
            } else if constexpr (COLOR == PieceColor::BLACK) {
                if ((8 <= src) && (src < 16)) {
                    for (const std::uint64_t dst : destinations) {
                        ChessBoard next = *this;
                        next.clear_square(src);
                        next.clear_square(dst);
                        next.add_piece<COLOR, PieceType::QUEEN>(dst);
                        visitor.template visit_promotion<PieceType::QUEEN>(
                            *this, next, src, dst,
                            next.visit<Visitor, other(COLOR), DEPTH - 1>()
                        );
                        next.clear_square(dst);
                        next.add_piece<COLOR, PieceType::ROOK>(dst);
                        visitor.template visit_promotion<PieceType::ROOK>(
                            *this, next, src, dst,
                            next.visit<Visitor, other(COLOR), DEPTH - 1>()
                        );
                        next.clear_square(dst);
                        next.add_piece<COLOR, PieceType::BISHOP>(dst);
                        visitor.template visit_promotion<PieceType::BISHOP>(
                            *this, next, src, dst,
                            next.visit<Visitor, other(COLOR), DEPTH - 1>()
                        );
                        next.clear_square(dst);
                        next.add_piece<COLOR, PieceType::KNIGHT>(dst);
                        visitor.template visit_promotion<PieceType::KNIGHT>(
                            *this, next, src, dst,
                            next.visit<Visitor, other(COLOR), DEPTH - 1>()
                        );
                    }
                } else {
                    for (const std::uint64_t dst : destinations) {
                        ChessBoard next = *this;
                        next.clear_square(src);
                        next.clear_square(dst);
                        next.add_piece<COLOR, PieceType::PAWN>(dst);
                        visitor.template visit<PieceType::PAWN>(
                            *this, next, src, dst,
                            next.visit<Visitor, other(COLOR), DEPTH - 1>()
                        );
                    }
                }
            }
        }
    }

    template <template <PieceColor, int> typename Visitor,
              PieceColor COLOR, int DEPTH>
    constexpr Visitor<COLOR, DEPTH>::result_type visit() const noexcept {
        if constexpr (DEPTH == 0) {
            return Visitor<COLOR, DEPTH>::visit(*this);
        } else {
            Visitor<COLOR, DEPTH> v{};
            visit_piece_moves<Visitor, COLOR, DEPTH, PieceType::KING  >(v);
            visit_piece_moves<Visitor, COLOR, DEPTH, PieceType::QUEEN >(v);
            visit_piece_moves<Visitor, COLOR, DEPTH, PieceType::ROOK  >(v);
            visit_piece_moves<Visitor, COLOR, DEPTH, PieceType::BISHOP>(v);
            visit_piece_moves<Visitor, COLOR, DEPTH, PieceType::KNIGHT>(v);
            visit_pawn_moves <Visitor, COLOR, DEPTH                   >(v);
            return v.get_result();
        }
    }

    template <template <PieceColor, int> typename Visitor,
              PieceColor COLOR, int DEPTH>
    constexpr Visitor<COLOR, DEPTH>::result_type
    visit(Visitor<COLOR, DEPTH> &v) const noexcept {
        visit_piece_moves<Visitor, COLOR, DEPTH, PieceType::KING  >(v);
        visit_piece_moves<Visitor, COLOR, DEPTH, PieceType::QUEEN >(v);
        visit_piece_moves<Visitor, COLOR, DEPTH, PieceType::ROOK  >(v);
        visit_piece_moves<Visitor, COLOR, DEPTH, PieceType::BISHOP>(v);
        visit_piece_moves<Visitor, COLOR, DEPTH, PieceType::KNIGHT>(v);
        visit_pawn_moves <Visitor, COLOR, DEPTH                   >(v);
        return v.get_result();
    }

}; // class ChessBoard


} // namespace DZChess

#endif // DZCHESS_CHESS_BOARD_HPP_INCLUDED
