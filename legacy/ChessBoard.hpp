#ifndef DZCHESS_CHESS_BOARD_HPP_INCLUDED
#define DZCHESS_CHESS_BOARD_HPP_INCLUDED

#include <array>          // for std::array
#include <compare>        // for operator<=>
#include <cstddef>        // for std::size_t
#include <cstdint>        // for std::int_fast8_t
#include <ostream>        // for std::ostream
#include <stdexcept>      // for std::invalid_argument

#include "ChessPiece.hpp" // for ChessPiece and PieceType


namespace DZChess {


    using coord_t = std::int_fast8_t;


    struct ChessDisplacement {
        coord_t d_rank;
        coord_t d_file;
    }; // struct ChessDisplacement


    constexpr coord_t BOARD_HEIGHT = 8;
    constexpr coord_t BOARD_WIDTH = 8;

    constexpr char MIN_RANK = '1';
    constexpr char MIN_FILE = 'a';

    constexpr char MAX_RANK = MIN_RANK + BOARD_HEIGHT - 1;
    constexpr char MAX_FILE = MIN_FILE + BOARD_WIDTH - 1;


    constexpr bool is_valid_rank(char rank) noexcept {
        return (MIN_RANK <= rank) && (rank <= MAX_RANK);
    }

    constexpr bool is_valid_file(char file) noexcept {
        return (MIN_FILE <= file) && (file <= MAX_FILE);
    }

    constexpr void assert_valid_rank(char rank) {
        if (!is_valid_rank(rank)) {
            throw std::invalid_argument("invalid rank");
        }
    }

    constexpr void assert_valid_file(char file) {
        if (!is_valid_file(file)) {
            throw std::invalid_argument("invalid file");
        }
    }


    class ChessSquare {

    private:

        coord_t _rank;
        coord_t _file;

    public:

        explicit constexpr ChessSquare() noexcept :
            _rank(0), _file(0) {}

        explicit constexpr ChessSquare(coord_t rank, coord_t file) noexcept :
            _rank(rank), _file(file) {}

        constexpr coord_t rank() const noexcept { return _rank; }
        constexpr coord_t file() const noexcept { return _file; }

        constexpr auto operator<=>(const ChessSquare &) const noexcept = default;

        constexpr bool in_bounds() const noexcept {
            return ((0 <= _rank) && (_rank < BOARD_HEIGHT) &&
                    (0 <= _file) && (_file < BOARD_WIDTH));
        }

        constexpr ChessSquare &operator+=(ChessDisplacement displ) noexcept {
            _rank += displ.d_rank;
            _file += displ.d_file;
            return *this;
        }

        constexpr ChessSquare operator+(ChessDisplacement displ) const noexcept {
            ChessSquare copy(*this);
            copy += displ;
            return copy;
        }

        friend std::ostream &operator<<(std::ostream &os, const ChessSquare &square) {
            os << static_cast<char>(MIN_FILE + square.file());
            os << static_cast<char>(MIN_RANK + square.rank());
            return os;
        }

    }; // class ChessSquare


    class ChessMove {

    private:

        ChessSquare _source;
        ChessSquare _destination;
        PieceType _promotion_type;

    public:

        explicit constexpr ChessMove() noexcept :
            _source(), _destination(), _promotion_type(PieceType::NONE) {}

        explicit constexpr ChessMove(const ChessSquare &source,
                                     const ChessSquare &destination) noexcept :
            _source(source), _destination(destination),
            _promotion_type(PieceType::NONE) {}

        explicit constexpr ChessMove(const ChessSquare &source,
                                     const ChessSquare &destination,
                                     PieceType promotion_type) noexcept :
            _source(source), _destination(destination),
            _promotion_type(promotion_type) {}

        explicit constexpr ChessMove(const char *str) {
            assert_valid_file(str[0]);
            assert_valid_rank(str[1]);
            assert_valid_file(str[2]);
            assert_valid_rank(str[3]);
            _source = ChessSquare(static_cast<coord_t>(str[1] - MIN_RANK),
                                  static_cast<coord_t>(str[0] - MIN_FILE));
            _destination = ChessSquare(static_cast<coord_t>(str[3] - MIN_RANK),
                                       static_cast<coord_t>(str[2] - MIN_FILE));
            if      (str[4] == 'q') { _promotion_type = PieceType::QUEEN; }
            else if (str[4] == 'r') { _promotion_type = PieceType::ROOK; }
            else if (str[4] == 'b') { _promotion_type = PieceType::BISHOP; }
            else if (str[4] == 'k') { _promotion_type = PieceType::KNIGHT; }
            else                    { _promotion_type = PieceType::NONE; }
            if ((str[4] != '\0') && (str[5] != '\0')) {
                throw std::invalid_argument("string passed to ChessMove "
                                            "is longer than expected");
            }
        }

        constexpr ChessSquare source        () const noexcept { return _source        ; }
        constexpr ChessSquare destination   () const noexcept { return _destination   ; }
        constexpr PieceType   promotion_type() const noexcept { return _promotion_type; }

        constexpr auto operator<=>(const ChessMove &) const noexcept = default;

        constexpr bool affects(ChessSquare square) const noexcept {
            return (_source == square) || (_destination == square);
        }

        friend std::ostream &operator<<(std::ostream &os, const ChessMove &move) {
            os << move.source();
            os << move.destination();
            switch (move.promotion_type()) {
                case PieceType::NONE  : {            break; }
                case PieceType::QUEEN : { os << 'q'; break; }
                case PieceType::ROOK  : { os << 'r'; break; }
                case PieceType::BISHOP: { os << 'b'; break; }
                case PieceType::KNIGHT: { os << 'n'; break; }
                case PieceType::KING: {
                    throw std::invalid_argument("cannot promote to king");
                }
                case PieceType::PAWN: {
                    throw std::invalid_argument("cannot promote to pawn");
                }
            }
            return os;
        }

    };


    class ChessBoard {

    private:

        std::array<std::array<ChessPiece, BOARD_WIDTH>, BOARD_HEIGHT> _data;

    public:

        explicit constexpr ChessBoard() noexcept :
            _data({{{{BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN,
                      BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK}},
                    {{BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN,
                      BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN}},
                    {{EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE,
                      EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE}},
                    {{EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE,
                      EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE}},
                    {{EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE,
                      EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE}},
                    {{EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE,
                      EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE}},
                    {{WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN,
                      WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN}},
                    {{WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN,
                      WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK}}}}) {}

        constexpr auto operator<=>(const ChessBoard &) const noexcept = default;

        constexpr ChessPiece &operator[](ChessSquare square) {
            if (!(0 <= square.rank() && square.rank() < BOARD_HEIGHT)) {
                throw std::invalid_argument("rank out of range");
            }
            if (!(0 <= square.file() && square.file() < BOARD_WIDTH)) {
                throw std::invalid_argument("file out of range");
            }
            return _data[BOARD_HEIGHT - square.rank() - 1][square.file()];
        }

        constexpr const ChessPiece &operator[](ChessSquare square) const {
            if (!(0 <= square.rank() && square.rank() < BOARD_HEIGHT)) {
                throw std::invalid_argument("rank out of range");
            }
            if (!(0 <= square.file() && square.file() < BOARD_WIDTH)) {
                throw std::invalid_argument("file out of range");
            }
            return _data[BOARD_HEIGHT - square.rank() - 1][square.file()];
        }

        constexpr void make_move(const ChessMove &move) {
            const auto src = move.source();
            const auto dst = move.destination();
            (*this)[dst] = (*this)[src].promote(move.promotion_type());
            (*this)[src] = EMPTY_SQUARE;
        }

        constexpr void make_move(ChessSquare source, ChessSquare destination) {
            make_move(ChessMove{source, destination});
        }

        constexpr std::size_t hash() const noexcept {
            constexpr std::size_t p = 23;
            std::size_t result = 0;
            for (coord_t rank = 0; rank < BOARD_HEIGHT; ++rank) {
                for (coord_t file = 0; file < BOARD_WIDTH; ++file) {
                    result = p * result + _data[rank][file].hash();
                }
            }
            return result;
        }

        friend std::ostream &operator<<(std::ostream &os, const ChessBoard &board) {
            os << "    +-------------------------+" << std::endl;
            for (const auto &rank : board._data) {
                bool first = true;
                os << "    | ";
                for (const ChessPiece piece : rank) {
                    if (first) {
                        first = false;
                    } else {
                        os << ' ';
                    }
                    os << piece;
                }
                os << " |" << std::endl;
            }
            os << "    +-------------------------+" << std::endl;
            return os;
        }

    }; // class ChessBoard


} // namespace DZChess


#endif // DZCHESS_CHESS_BOARD_HPP_INCLUDED
