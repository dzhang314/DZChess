#ifndef DZCHESS_CHESS_PIECE_HPP_INCLUDED
#define DZCHESS_CHESS_PIECE_HPP_INCLUDED

#include <array>   // for std::array
#include <cstdint> // for std::uint64_t


enum class PieceColor { WHITE, BLACK };
enum class PieceType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };


constexpr PieceColor other(PieceColor color) noexcept {
    if (color == PieceColor::WHITE) {
        return PieceColor::BLACK;
    } else {
        return PieceColor::WHITE;
    }
}


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


constexpr std::array<char, 3> square_name(std::uint64_t square) noexcept {
    return {
        static_cast<char>('a' + (square % 8)),
        static_cast<char>('1' + (square / 8)),
       '\0'
    };
}


#endif // DZCHESS_CHESS_PIECE_HPP_INCLUDED
