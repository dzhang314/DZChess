#ifndef DZCHESS_CHESS_PIECE_HPP_INCLUDED
#define DZCHESS_CHESS_PIECE_HPP_INCLUDED

#include <compare> // for operator<=>
#include <cstddef> // for std::size_t
#include <cstdint> // for std::uint_fast8_t
#include <ostream> // for std::ostream


namespace DZChess {


    enum class PieceColor : std::uint_fast8_t {
        NONE, WHITE, BLACK
    }; // enum class PieceColor


    enum class PieceType : std::uint_fast8_t {
        NONE, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN
    }; // enum class PieceType


    class ChessPiece {

    private:

        PieceColor _color;
        PieceType _type;

    public:

        explicit constexpr ChessPiece() noexcept :
            _color(PieceColor::NONE), _type(PieceType::NONE) {}

        explicit constexpr ChessPiece(PieceColor color, PieceType type) noexcept :
            _color(color), _type(type) {}

        constexpr PieceColor color() const noexcept { return _color; }
        constexpr PieceType  type () const noexcept { return _type ; }

        constexpr auto operator<=>(const ChessPiece &) const noexcept = default;

        constexpr ChessPiece promote(PieceType type) const noexcept {
            return (type == PieceType::NONE) ? (*this) : ChessPiece(_color, type);
        }

        constexpr std::size_t hash() const noexcept {
            return (7 * static_cast<std::size_t>(_color) +
                    static_cast<std::size_t>(_type));
        }

        friend std::ostream& operator<<(std::ostream& os, const ChessPiece& piece) {
            switch (piece.color()) {
                case PieceColor::NONE:  { os << '['; break; }
                case PieceColor::WHITE: { os << 'w'; break; }
                case PieceColor::BLACK: { os << 'b'; break; }
            }
            switch (piece.type()) {
                case PieceType::NONE:   { os << ']'; break; }
                case PieceType::KING:   { os << 'K'; break; }
                case PieceType::QUEEN:  { os << 'Q'; break; }
                case PieceType::ROOK:   { os << 'R'; break; }
                case PieceType::BISHOP: { os << 'B'; break; }
                case PieceType::KNIGHT: { os << 'N'; break; }
                case PieceType::PAWN:   { os << 'p'; break; }
            }
            return os;
        }

    }; // class ChessPiece


    constexpr ChessPiece EMPTY_SQUARE{PieceColor::NONE , PieceType::NONE  };

    constexpr ChessPiece WHITE_KING  {PieceColor::WHITE, PieceType::KING  };
    constexpr ChessPiece WHITE_QUEEN {PieceColor::WHITE, PieceType::QUEEN };
    constexpr ChessPiece WHITE_ROOK  {PieceColor::WHITE, PieceType::ROOK  };
    constexpr ChessPiece WHITE_BISHOP{PieceColor::WHITE, PieceType::BISHOP};
    constexpr ChessPiece WHITE_KNIGHT{PieceColor::WHITE, PieceType::KNIGHT};
    constexpr ChessPiece WHITE_PAWN  {PieceColor::WHITE, PieceType::PAWN  };

    constexpr ChessPiece BLACK_KING  {PieceColor::BLACK, PieceType::KING  };
    constexpr ChessPiece BLACK_QUEEN {PieceColor::BLACK, PieceType::QUEEN };
    constexpr ChessPiece BLACK_ROOK  {PieceColor::BLACK, PieceType::ROOK  };
    constexpr ChessPiece BLACK_BISHOP{PieceColor::BLACK, PieceType::BISHOP};
    constexpr ChessPiece BLACK_KNIGHT{PieceColor::BLACK, PieceType::KNIGHT};
    constexpr ChessPiece BLACK_PAWN  {PieceColor::BLACK, PieceType::PAWN  };


} // namespace DZChess


#endif // DZCHESS_CHESS_PIECE_HPP_INCLUDED
