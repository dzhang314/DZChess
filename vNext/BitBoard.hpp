#ifndef DZCHESS_BIT_BOARD_HPP_INCLUDED
#define DZCHESS_BIT_BOARD_HPP_INCLUDED

#include <bit>     // for std::countr_zero, std::popcount
#include <cstdint> // for std::uint64_t

#include "ChessPiece.hpp"
#include "MoveTables.hpp"

namespace DZChess {


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


} // namespace DZChess

#endif // DZCHESS_BIT_BOARD_HPP_INCLUDED
