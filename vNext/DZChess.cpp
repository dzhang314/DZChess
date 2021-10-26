#include <bit>
#include <climits>
#include <cstdint>
#include <iostream>

#include "MoveTables.hpp"


enum class PieceColor { WHITE, BLACK };
enum class PieceType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };


template <PieceColor COLOR>
constexpr const char *COLOR_NAME = "[UNKNOWN COLOR]";
template <> constexpr const char *COLOR_NAME<PieceColor::WHITE> = "white";
template <> constexpr const char *COLOR_NAME<PieceColor::BLACK> = "black";


template <PieceType TYPE>
constexpr const char *TYPE_NAME = "[UNKNOWN TYPE]";
template <> constexpr const char *TYPE_NAME<PieceType::KING  > = "king"  ;
template <> constexpr const char *TYPE_NAME<PieceType::QUEEN > = "queen" ;
template <> constexpr const char *TYPE_NAME<PieceType::ROOK  > = "rook"  ;
template <> constexpr const char *TYPE_NAME<PieceType::BISHOP> = "bishop";
template <> constexpr const char *TYPE_NAME<PieceType::KNIGHT> = "knight";
template <> constexpr const char *TYPE_NAME<PieceType::PAWN  > = "pawn"  ;


class BitBoardIterator {

    std::uint64_t data;

public:

    explicit constexpr BitBoardIterator(std::uint64_t data) noexcept :
        data(data) {}

    constexpr bool operator==(BitBoardIterator other) const noexcept {
        return data == other.data;
    }

    constexpr BitBoardIterator &operator++() noexcept {
        data &= data - 1;
        return *this;
    }

    constexpr std::uint64_t operator*() const noexcept {
        return static_cast<std::uint64_t>(std::countr_zero(data));
    }

}; // class BitBoardIterator


class BitBoard {

    std::uint64_t data;

public:

    constexpr BitBoard(std::uint64_t data) noexcept : data(data) {}

    constexpr BitBoardIterator begin() const noexcept {
        return BitBoardIterator{data};
    }

    constexpr BitBoardIterator end() const noexcept {
        return BitBoardIterator{0};
    }

    constexpr BitBoard operator&(BitBoard rhs) const noexcept {
        return BitBoard{data & rhs.data};
    }

    constexpr BitBoard &operator&=(BitBoard rhs) noexcept {
        data &= rhs.data;
        return *this;
    }

    constexpr BitBoard operator|(BitBoard rhs) const noexcept {
        return BitBoard{data | rhs.data};
    }

    constexpr BitBoard &operator|=(BitBoard rhs) noexcept {
        data |= rhs.data;
        return *this;
    }

    constexpr BitBoard operator~() const noexcept {
        return BitBoard{~data};
    }

    constexpr BitBoard shift_up() const noexcept {
        return BitBoard{data << 8};
    }

    constexpr BitBoard shift_down() const noexcept {
        return BitBoard{data >> 8};
    }

    constexpr int popcount() const noexcept {
        return std::popcount(data);
    }

    constexpr BitBoard king_moves(std::uint64_t src,
                                  BitBoard own_pieces) const noexcept {
        return BitBoard{KING_MOVE_TABLE[src]} & ~own_pieces;
    }

    constexpr BitBoard queen_moves(std::uint64_t src,
                                   BitBoard own_pieces) const noexcept {
        const std::uint64_t r_blockers = data & ROOK_MASK_TABLE[src];
        const std::uint64_t b_blockers = data & BISHOP_MASK_TABLE[src];
        std::uint64_t r_index = r_blockers * ROOK_MAGIC_NUMBER_TABLE[src];
        std::uint64_t b_index = b_blockers * BISHOP_MAGIC_NUMBER_TABLE[src];
        r_index >>= ROOK_MAGIC_BIT_COUNT;
        b_index >>= BISHOP_MAGIC_BIT_COUNT;
        r_index += src << (64 - ROOK_MAGIC_BIT_COUNT);
        b_index += src << (64 - BISHOP_MAGIC_BIT_COUNT);
        return BitBoard{ROOK_MAGIC_MOVE_TABLE[r_index] |
                        BISHOP_MAGIC_MOVE_TABLE[b_index]} & ~own_pieces;
    }

    constexpr BitBoard rook_moves(std::uint64_t src,
                                  BitBoard own_pieces) const noexcept {
        const std::uint64_t blockers = data & ROOK_MASK_TABLE[src];
        std::uint64_t index = blockers * ROOK_MAGIC_NUMBER_TABLE[src];
        index >>= ROOK_MAGIC_BIT_COUNT;
        index += src << (64 - ROOK_MAGIC_BIT_COUNT);
        return BitBoard{ROOK_MAGIC_MOVE_TABLE[index]} & ~own_pieces;
    }

    constexpr BitBoard bishop_moves(std::uint64_t src,
                                    BitBoard own_pieces) const noexcept {
        const std::uint64_t blockers = data & BISHOP_MASK_TABLE[src];
        std::uint64_t index = blockers * BISHOP_MAGIC_NUMBER_TABLE[src];
        index >>= BISHOP_MAGIC_BIT_COUNT;
        index += src << (64 - BISHOP_MAGIC_BIT_COUNT);
        return BitBoard{BISHOP_MAGIC_MOVE_TABLE[index]} & ~own_pieces;
    }

    constexpr BitBoard knight_moves(std::uint64_t src,
                                    BitBoard own_pieces) const noexcept {
        return BitBoard{KNIGHT_MOVE_TABLE[src]} & ~own_pieces;
    }

    template <PieceColor COLOR>
    constexpr BitBoard pawn_moves(std::uint64_t src,
                                  BitBoard own_pieces) const noexcept {
        const BitBoard empty = this->operator~();
        const BitBoard opp_pieces = this->operator&(~own_pieces);
        if constexpr (COLOR == PieceColor::WHITE) {
            const BitBoard double_empty = empty & empty.shift_up();
            const BitBoard moves{WHITE_PAWN_MOVE_TABLE[src]};
            const BitBoard captures{WHITE_PAWN_CAPTURE_TABLE[src]};
            const BitBoard double_moves{WHITE_PAWN_DOUBLE_MOVE_TABLE[src]};
            return ((moves & empty) |
                    (captures & opp_pieces) |
                    (double_moves & double_empty));
        } else if constexpr (COLOR == PieceColor::BLACK) {
            const BitBoard double_empty = empty & empty.shift_down();
            const BitBoard moves{BLACK_PAWN_MOVE_TABLE[src]};
            const BitBoard captures{BLACK_PAWN_CAPTURE_TABLE[src]};
            const BitBoard double_moves{BLACK_PAWN_DOUBLE_MOVE_TABLE[src]};
            return ((moves & empty) |
                    (captures & opp_pieces) |
                    (double_moves & double_empty));
        }
    }

    template <PieceColor COLOR, PieceType TYPE>
    constexpr BitBoard moves(std::uint64_t src,
                             BitBoard own_pieces) const noexcept {
        if constexpr (TYPE == PieceType::KING) {
            return king_moves(src, own_pieces);
        } else if constexpr (TYPE == PieceType::QUEEN) {
            return queen_moves(src, own_pieces);
        } else if constexpr (TYPE == PieceType::ROOK) {
            return rook_moves(src, own_pieces);
        } else if constexpr (TYPE == PieceType::BISHOP) {
            return bishop_moves(src, own_pieces);
        } else if constexpr (TYPE == PieceType::KNIGHT) {
            return knight_moves(src, own_pieces);
        } else if constexpr (TYPE == PieceType::PAWN) {
            return pawn_moves<COLOR>(src, own_pieces);
        }
    }

}; // class BitBoard


constexpr std::array<char, 3> square_name(std::uint64_t square) noexcept {
    return {
        static_cast<char>('a' + (square % 8)),
        static_cast<char>('1' + (square / 8)),
       '\0'
    };
}


constexpr PieceColor other(PieceColor color) noexcept {
    if (color == PieceColor::WHITE) {
        return PieceColor::BLACK;
    } else {
        return PieceColor::WHITE;
    }
}


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
        if constexpr (COLOR == PieceColor::WHITE) {
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

}; // class ChessBoard


                // std::cout << COLOR_NAME<color> << ' ' << TYPE_NAME<type>
                //           << " at " << square_name(src).data()
                //           << " can move to " << square_name(dst).data()
                //           << std::endl;


template <PieceColor COLOR, int DEPTH>
struct MaterialisticEvaluationVisitor {

    int accumulator;

    using result_type = int;

    static constexpr int visit(const ChessBoard &b) noexcept {
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

};


int main() {
    ChessBoard board{};
    std::cout << board.visit<MaterialisticEvaluationVisitor, PieceColor::WHITE, 5>() << std::endl;
}
