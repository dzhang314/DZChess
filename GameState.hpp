#ifndef DZCHESS_GAME_STATE_HPP_INCLUDED
#define DZCHESS_GAME_STATE_HPP_INCLUDED

#include <compare> // for operator<=>
#include <sstream> // for std::ostringstream
#include <string>  // for std::string
#include <utility> // for std::pair
#include <vector>  // for std::vector

#include "ChessPiece.hpp"
#include "ChessBoard.hpp"


namespace DZChess {


    class GameState {

    private: // ================================================================== MEMBER VARIABLES

        ChessBoard _board;
        ChessSquare _white_king_location;
        ChessSquare _black_king_location;
        PieceColor _color_to_move;
        coord_t _en_passant_file;
        bool _white_short_castle_available;
        bool _white_long_castle_available;
        bool _black_short_castle_available;
        bool _black_long_castle_available;

    public: // ======================================================================= CONSTRUCTORS

        explicit constexpr GameState() noexcept :
            _board(),                            // starting position
            _white_king_location(0, 4),          // white king starts on e1
            _black_king_location(7, 4),          // black king starts on e8
            _color_to_move(PieceColor::WHITE),   // white goes first
            _en_passant_file(-1),                // en passant not available
            _white_short_castle_available(true), // all castling available
            _white_long_castle_available(true),
            _black_short_castle_available(true),
            _black_long_castle_available(true) {}

    public: // ========================================================================== ACCESSORS

        constexpr const ChessBoard &board() const noexcept { return _board; }
        constexpr PieceColor color_to_move() const noexcept { return _color_to_move; }
        constexpr coord_t en_passant_file() const noexcept { return _en_passant_file; }

        constexpr auto operator<=>(const GameState &) const noexcept = default;

    private: // ============================================================== QUERYING BOARD STATE

        constexpr PieceColor opponent() const noexcept {
            return (_color_to_move == PieceColor::BLACK)
                ? PieceColor::WHITE : PieceColor::BLACK;
        }

        constexpr bool is_empty(ChessSquare square) const noexcept {
            return square.in_bounds() && (_board[square].type() == PieceType::NONE);
        }

        constexpr bool has_own_piece(ChessSquare square) const noexcept {
            return square.in_bounds() && (_board[square].color() == _color_to_move);
        }

        constexpr bool has_enemy_piece(ChessSquare square) const noexcept {
            if (!square.in_bounds()) { return false; }
            const ChessPiece piece = _board[square];
            return (piece.type() != PieceType::NONE) && (piece.color() != _color_to_move);
        }

        constexpr bool has_enemy_queen_or_rook(ChessSquare square) const noexcept {
            if (!square.in_bounds()) { return false; }
            const ChessPiece piece = _board[square];
            const PieceType type = piece.type();
            return ((piece.color() != _color_to_move) &&
                    ((type == PieceType::QUEEN) || (type == PieceType::ROOK)));
        }

        constexpr bool has_enemy_queen_or_bishop(ChessSquare square) const noexcept {
            if (!square.in_bounds()) { return false; }
            const ChessPiece piece = _board[square];
            const PieceType type = piece.type();
            return ((piece.color() != _color_to_move) &&
                    ((type == PieceType::QUEEN) || (type == PieceType::BISHOP)));
        }

        constexpr bool has_enemy_knight(ChessSquare square) const noexcept {
            if (!square.in_bounds()) { return false; }
            const ChessPiece piece = _board[square];
            return ((piece.color() != _color_to_move) &&
                    (piece.type() == PieceType::KNIGHT));
        }

        constexpr bool has_enemy_pawn(ChessSquare square) const noexcept {
            if (!square.in_bounds()) { return false; }
            const ChessPiece piece = _board[square];
            return ((piece.color() != _color_to_move) &&
                    (piece.type() == PieceType::PAWN));
        }

        constexpr bool has_enemy_king(ChessSquare square) const noexcept {
            if (!square.in_bounds()) { return false; }
            const ChessPiece piece = _board[square];
            return ((piece.color() != _color_to_move) &&
                    (piece.type() == PieceType::KING));
        }

        constexpr bool can_move_to(ChessSquare square) const noexcept {
            // A square can be moved to if it is empty or contains an enemy piece.
            return square.in_bounds() && (_board[square].color() != _color_to_move);
        }

    private: // ============================================================ CLASSIFYING MOVE TYPES

        constexpr bool is_short_castle(const ChessMove &move) const {
            const ChessPiece piece = _board[move.source()];
            return ((piece.type() == PieceType::KING) &&
                    (move.source().file() == 4) &&
                    (move.destination().file() == 6));
        }

        constexpr bool is_long_castle(const ChessMove &move) const {
            const ChessPiece piece = _board[move.source()];
            return ((piece.type() == PieceType::KING) &&
                    (move.source().file() == 4) &&
                    (move.destination().file() == 2));
        }

        constexpr bool is_en_passant_capture(const ChessMove &move) const {
            const ChessSquare dst = move.destination();
            if (!is_empty(dst)) { return false; }
            const ChessSquare src = move.source();
            if (src.file() == dst.file()) { return false; }
            const PieceType type = _board[src].type();
            return (type == PieceType::PAWN);
        }

        constexpr bool is_capture(const ChessMove &move) const {
            return has_enemy_piece(move.destination()) || is_en_passant_capture(move);
        }

    private: // ====================================================================== MAKING MOVES

        constexpr void update_castling_state(const ChessMove &move) noexcept {
            if (move.affects(ChessSquare{0, 7})) {
                _white_short_castle_available = false;
            }
            if (move.affects(ChessSquare{0, 0})) {
                _white_long_castle_available = false;
            }
            if (move.affects(ChessSquare{0, 4})) {
                _white_short_castle_available = false;
                _white_long_castle_available = false;
            }
            if (move.affects(ChessSquare{7, 7})) {
                _black_short_castle_available = false;
            }
            if (move.affects(ChessSquare{7, 0})) {
                _black_long_castle_available = false;
            }
            if (move.affects(ChessSquare{7, 4})) {
                _black_short_castle_available = false;
                _black_long_castle_available = false;
            }
        }

    public:

        constexpr void make_move(const ChessMove &move) {
            // Note that make_move does not fully validate that the given move is legal.
            // It only performs some basic checks.
            const ChessSquare src = move.source();
            const ChessSquare dst = move.destination();
            const PieceType type = _board[src].type();
            if (!has_own_piece(src)) {
                throw std::invalid_argument("attempted to move invalid piece");
            }
            if (!can_move_to(dst)) {
                throw std::invalid_argument("attempted to move to invalid square");
            }
            if (is_en_passant_capture(move)) {
                if (dst.file() != _en_passant_file) {
                    throw std::invalid_argument("invalid en passant");
                }
                _board[ChessSquare{src.rank(), dst.file()}] = EMPTY_SQUARE;
            }
            if (is_short_castle(move)) {
                _board.make_move(ChessSquare{src.rank(), 7},
                                 ChessSquare{src.rank(), 5});
            }
            if (is_long_castle(move)) {
                _board.make_move(ChessSquare{src.rank(), 0},
                                 ChessSquare{src.rank(), 3});
            }
            _board.make_move(move);
            if (type == PieceType::KING) {
                if (_color_to_move == PieceColor::BLACK) {
                    _black_king_location = dst;
                } else {
                    _white_king_location = dst;
                }
            }
            _color_to_move = opponent();
            if ((type == PieceType::PAWN) &&
                ((src.rank() - dst.rank() == +2) ||
                 (src.rank() - dst.rank() == -2))) {
                _en_passant_file = dst.file();
            } else {
                _en_passant_file = -1;
            }
            update_castling_state(move);
        }

    private: // ============================================================= GENERATING PAWN MOVES

        constexpr coord_t pawn_origin_rank() const noexcept {
            return (_color_to_move == PieceColor::WHITE) ? 1 : (BOARD_HEIGHT - 2);
        }

        constexpr coord_t promotion_rank() const noexcept {
            return (_color_to_move == PieceColor::WHITE) ? (BOARD_HEIGHT - 1) : 0;
        }

        constexpr coord_t en_passant_rank() const noexcept {
            return (_color_to_move == PieceColor::WHITE) ? (BOARD_HEIGHT - 4) : 3;
        }

        constexpr bool can_en_passant(ChessSquare source,
                                      ChessSquare destination) const noexcept {
            return ((source.rank() == en_passant_rank()) &&
                    (destination.file() == _en_passant_file) &&
                    is_empty(destination));
        }

        constexpr void promotion_moves(
            std::vector<ChessMove> &moves,
            ChessSquare source,
            ChessSquare destination
        ) const noexcept {
            if (destination.rank() == promotion_rank()) {
                moves.emplace_back(source, destination, PieceType::QUEEN);
                moves.emplace_back(source, destination, PieceType::ROOK);
                moves.emplace_back(source, destination, PieceType::BISHOP);
                moves.emplace_back(source, destination, PieceType::KNIGHT);
            } else {
                moves.emplace_back(source, destination);
            }
        }

        constexpr void pawn_moves(
            std::vector<ChessMove> &moves,
            ChessSquare source
        ) const noexcept {
            const coord_t up = (_color_to_move == PieceColor::BLACK) ? -1 : +1;
            const ChessSquare one = source + ChessDisplacement{up, 0};
            if (is_empty(one)) {
                promotion_moves(moves, source, one);
                const ChessSquare two = one + ChessDisplacement{up, 0};
                if ((source.rank() == pawn_origin_rank()) && is_empty(two)) {
                    promotion_moves(moves, source, two);
                }
            }
            const ChessSquare ldiag = source + ChessDisplacement{up, -1};
            if (has_enemy_piece(ldiag) || can_en_passant(source, ldiag)) {
                promotion_moves(moves, source, ldiag);
            }
            const ChessSquare rdiag = source + ChessDisplacement{up, +1};
            if (has_enemy_piece(rdiag) || can_en_passant(source, rdiag)) {
                promotion_moves(moves, source, rdiag);
            }
        }

    private: // ============================================================ GENERATING PIECE MOVES

        constexpr void leaper_move(
            std::vector<ChessMove> &moves,
            ChessSquare source,
            ChessDisplacement displacement
        ) const noexcept {
            ChessSquare destination = source + displacement;
            if (can_move_to(destination)) {
                moves.emplace_back(source, destination);
            }
        }

        constexpr void slider_moves(
            std::vector<ChessMove> &moves,
            ChessSquare source,
            ChessDisplacement displacement
        ) const noexcept {
            ChessSquare destination = source + displacement;
            while (is_empty(destination)) {
                moves.emplace_back(source, destination);
                destination += displacement;
            }
            if (has_enemy_piece(destination)) {
                moves.emplace_back(source, destination);
            }
        }

        constexpr void available_moves_ignoring_check(
            std::vector<ChessMove> &moves,
            const ChessSquare &source,
            PieceColor color
        ) const noexcept {
            if (source.in_bounds()) {
                const ChessPiece piece = _board[source];
                if (piece.color() == color) {
                    switch (piece.type()) {
                        case PieceType::NONE: break;
                        case PieceType::KING:
                            leaper_move(moves, source, {+1, 0});
                            leaper_move(moves, source, {-1, 0});
                            leaper_move(moves, source, {0, +1});
                            leaper_move(moves, source, {0, -1});
                            leaper_move(moves, source, {+1, +1});
                            leaper_move(moves, source, {+1, -1});
                            leaper_move(moves, source, {-1, +1});
                            leaper_move(moves, source, {-1, -1});
                            break;
                        case PieceType::QUEEN:
                            slider_moves(moves, source, {+1, 0});
                            slider_moves(moves, source, {-1, 0});
                            slider_moves(moves, source, {0, +1});
                            slider_moves(moves, source, {0, -1});
                            slider_moves(moves, source, {+1, +1});
                            slider_moves(moves, source, {+1, -1});
                            slider_moves(moves, source, {-1, +1});
                            slider_moves(moves, source, {-1, -1});
                            break;
                        case PieceType::ROOK:
                            slider_moves(moves, source, {+1, 0});
                            slider_moves(moves, source, {-1, 0});
                            slider_moves(moves, source, {0, +1});
                            slider_moves(moves, source, {0, -1});
                            break;
                        case PieceType::BISHOP:
                            slider_moves(moves, source, {+1, +1});
                            slider_moves(moves, source, {+1, -1});
                            slider_moves(moves, source, {-1, +1});
                            slider_moves(moves, source, {-1, -1});
                            break;
                        case PieceType::KNIGHT:
                            leaper_move(moves, source, {+1, +2});
                            leaper_move(moves, source, {+1, -2});
                            leaper_move(moves, source, {-1, +2});
                            leaper_move(moves, source, {-1, -2});
                            leaper_move(moves, source, {+2, +1});
                            leaper_move(moves, source, {+2, -1});
                            leaper_move(moves, source, {-2, +1});
                            leaper_move(moves, source, {-2, -1});
                            break;
                        case PieceType::PAWN:
                            pawn_moves(moves, source);
                            break;
                    }
                }
            }
        }

        std::vector<ChessMove>
        available_moves_ignoring_check(PieceColor color) const noexcept {
            std::vector<ChessMove> result;
            for (coord_t rank = 0; rank < BOARD_HEIGHT; ++rank) {
                for (coord_t file = 0; file < BOARD_WIDTH; ++file) {
                    const ChessSquare source{rank, file};
                    available_moves_ignoring_check(result, source, color);
                }
            }
            return result;
        }

        std::vector<ChessMove>
        available_moves_ignoring_check() const noexcept {
            return available_moves_ignoring_check(_color_to_move);
        }

    private: // ========================================================= GENERATING CASTLING MOVES

        constexpr bool orthogonal_check(ChessSquare source,
                                        ChessDisplacement displacement) const noexcept {
            ChessSquare square = source + displacement;
            while (is_empty(square)) { square += displacement; }
            return has_enemy_queen_or_rook(square);
        }

        constexpr bool diagonal_check(ChessSquare source,
                                      ChessDisplacement displacement) const noexcept {
            ChessSquare square = source + displacement;
            while (is_empty(square)) { square += displacement; }
            return has_enemy_queen_or_bishop(square);
        }

        constexpr bool is_attacked(ChessSquare square) const noexcept {
            const coord_t up = (_color_to_move == PieceColor::BLACK) ? -1 : +1;
            return (has_enemy_knight(square + ChessDisplacement{+1, +2}) ||
                    has_enemy_knight(square + ChessDisplacement{+1, -2}) ||
                    has_enemy_knight(square + ChessDisplacement{-1, +2}) ||
                    has_enemy_knight(square + ChessDisplacement{-1, -2}) ||
                    has_enemy_knight(square + ChessDisplacement{+2, +1}) ||
                    has_enemy_knight(square + ChessDisplacement{+2, -1}) ||
                    has_enemy_knight(square + ChessDisplacement{-2, +1}) ||
                    has_enemy_knight(square + ChessDisplacement{-2, -1}) ||
                    has_enemy_pawn(square + ChessDisplacement{up, +1}) ||
                    has_enemy_pawn(square + ChessDisplacement{up, -1}) ||
                    orthogonal_check(square, ChessDisplacement{0, +1}) ||
                    orthogonal_check(square, ChessDisplacement{0, -1}) ||
                    orthogonal_check(square, ChessDisplacement{+1, 0}) ||
                    orthogonal_check(square, ChessDisplacement{-1, 0}) ||
                    diagonal_check(square, ChessDisplacement{+1, +1}) ||
                    diagonal_check(square, ChessDisplacement{+1, -1}) ||
                    diagonal_check(square, ChessDisplacement{-1, +1}) ||
                    diagonal_check(square, ChessDisplacement{-1, -1}) ||
                    has_enemy_king(square + ChessDisplacement{0, +1}) ||
                    has_enemy_king(square + ChessDisplacement{0, -1}) ||
                    has_enemy_king(square + ChessDisplacement{+1, 0}) ||
                    has_enemy_king(square + ChessDisplacement{-1, 0}) ||
                    has_enemy_king(square + ChessDisplacement{+1, +1}) ||
                    has_enemy_king(square + ChessDisplacement{+1, -1}) ||
                    has_enemy_king(square + ChessDisplacement{-1, +1}) ||
                    has_enemy_king(square + ChessDisplacement{-1, -1}));
        }

        constexpr void castling_moves(std::vector<ChessMove> &moves) const {
            if (_color_to_move == PieceColor::WHITE) {
                if (_white_short_castle_available) {
                    if (_board[ChessSquare{0, 4}] != WHITE_KING) {
                        throw std::invalid_argument("king in wrong place to castle");
                    }
                    if (_board[ChessSquare{0, 7}] != WHITE_ROOK) {
                        throw std::invalid_argument("rook in wrong place to castle");
                    }
                    if (is_empty(ChessSquare{0, 5}) &&
                        is_empty(ChessSquare{0, 6}) &&
                        !is_attacked(ChessSquare{0, 4}) &&
                        !is_attacked(ChessSquare{0, 5}) &&
                        !is_attacked(ChessSquare{0, 6})) {
                        moves.emplace_back(ChessSquare{0, 4}, ChessSquare{0, 6});
                    }
                }
                if (_white_long_castle_available) {
                    if (_board[ChessSquare{0, 4}] != WHITE_KING) {
                        throw std::invalid_argument("king in wrong place to castle");
                    }
                    if (_board[ChessSquare{0, 0}] != WHITE_ROOK) {
                        throw std::invalid_argument("rook in wrong place to castle");
                    }
                    if (is_empty(ChessSquare{0, 1}) &&
                        is_empty(ChessSquare{0, 2}) &&
                        is_empty(ChessSquare{0, 3}) &&
                        !is_attacked(ChessSquare{0, 2}) &&
                        !is_attacked(ChessSquare{0, 3}) &&
                        !is_attacked(ChessSquare{0, 4})) {
                        moves.emplace_back(ChessSquare{0, 4}, ChessSquare{0, 2});
                    }
                }
            } else if (_color_to_move == PieceColor::BLACK) {
                if (_black_short_castle_available) {
                    if (_board[ChessSquare{7, 4}] != BLACK_KING) {
                        throw std::invalid_argument("king in wrong place to castle");
                    }
                    if (_board[ChessSquare{7, 7}] != BLACK_ROOK) {
                        throw std::invalid_argument("rook in wrong place to castle");
                    }
                    if (is_empty(ChessSquare{7, 5}) &&
                        is_empty(ChessSquare{7, 6}) &&
                        !is_attacked(ChessSquare{7, 4}) &&
                        !is_attacked(ChessSquare{7, 5}) &&
                        !is_attacked(ChessSquare{7, 6})) {
                        moves.emplace_back(ChessSquare{7, 4}, ChessSquare{7, 6});
                    }
                }
                if (_black_long_castle_available) {
                    if (_board[ChessSquare{7, 4}] != BLACK_KING) {
                        throw std::invalid_argument("king in wrong place to castle");
                    }
                    if (_board[ChessSquare{7, 0}] != BLACK_ROOK) {
                        throw std::invalid_argument("rook in wrong place to castle");
                    }
                    if (is_empty(ChessSquare{7, 1}) &&
                        is_empty(ChessSquare{7, 2}) &&
                        is_empty(ChessSquare{7, 3}) &&
                        !is_attacked(ChessSquare{7, 2}) &&
                        !is_attacked(ChessSquare{7, 3}) &&
                        !is_attacked(ChessSquare{7, 4})) {
                        moves.emplace_back(ChessSquare{7, 4}, ChessSquare{7, 2});
                    }
                }
            }
        }

    public: // ======================================================= DETECTING AND HANDLING CHECK

        constexpr bool in_check() const {
            const ChessSquare king_location =
                (_color_to_move == PieceColor::BLACK)
                ? _black_king_location : _white_king_location;
            const coord_t up = (_color_to_move == PieceColor::BLACK) ? -1 : +1;
            return (has_enemy_knight(king_location + ChessDisplacement{+1, +2}) ||
                    has_enemy_knight(king_location + ChessDisplacement{+1, -2}) ||
                    has_enemy_knight(king_location + ChessDisplacement{-1, +2}) ||
                    has_enemy_knight(king_location + ChessDisplacement{-1, -2}) ||
                    has_enemy_knight(king_location + ChessDisplacement{+2, +1}) ||
                    has_enemy_knight(king_location + ChessDisplacement{+2, -1}) ||
                    has_enemy_knight(king_location + ChessDisplacement{-2, +1}) ||
                    has_enemy_knight(king_location + ChessDisplacement{-2, -1}) ||
                    has_enemy_pawn(king_location + ChessDisplacement{up, +1}) ||
                    has_enemy_pawn(king_location + ChessDisplacement{up, -1}) ||
                    orthogonal_check(king_location, ChessDisplacement{0, +1}) ||
                    orthogonal_check(king_location, ChessDisplacement{0, -1}) ||
                    orthogonal_check(king_location, ChessDisplacement{+1, 0}) ||
                    orthogonal_check(king_location, ChessDisplacement{-1, 0}) ||
                    diagonal_check(king_location, ChessDisplacement{+1, +1}) ||
                    diagonal_check(king_location, ChessDisplacement{+1, -1}) ||
                    diagonal_check(king_location, ChessDisplacement{-1, +1}) ||
                    diagonal_check(king_location, ChessDisplacement{-1, -1}));
        }

    private:

        constexpr bool puts_self_in_check(const ChessMove &move) const {
            GameState next = *this;
            next.make_move(move);
            next._color_to_move = _color_to_move;
            return next.in_check();
        }

        constexpr bool puts_enemy_in_check(const ChessMove &move) const {
            GameState next = *this;
            next.make_move(move);
            return next.in_check();
        }

    public: // ========================================================= GENERATING ALL LEGAL MOVES

        std::vector<ChessMove> available_moves() const {
            std::vector<ChessMove> result;
            for (const ChessMove &move : available_moves_ignoring_check()) {
                if (!puts_self_in_check(move)) {
                    result.push_back(move);
                }
            }
            castling_moves(result);
            return result;
        }

        GameState after_move_checked(const ChessMove &move) const {
            for (const ChessMove &legal_move : available_moves()) {
                if (move == legal_move) {
                    GameState copy = *this;
                    copy.make_move(move);
                    return copy;
                }
            }
            throw std::invalid_argument("attempted to make illegal move");
        }

        GameState after_move_checked(const char *str) const {
            return after_move_checked(ChessMove{str});
        }

        constexpr GameState after_move(const ChessMove &move) const {
            GameState copy = *this;
            copy.make_move(move);
            return copy;
        }

        constexpr GameState after_move(const char *str) const {
            return after_move(ChessMove{str});
        }

    private: // ====================================================================== NAMING MOVES

        constexpr bool puts_enemy_in_checkmate(const ChessMove &move) const {
            if (!puts_enemy_in_check(move)) { return false; }
            GameState next = *this;
            next.make_move(move);
            return (next.available_moves().size() == 0);
        }

        constexpr bool puts_enemy_in_stalemate(const ChessMove &move) const {
            if (puts_enemy_in_check(move)) { return false; }
            GameState next = *this;
            next.make_move(move);
            return (next.available_moves().size() == 0);
        }

    public:

        std::vector<std::pair<ChessMove, std::string>>
        available_moves_and_names() const {
            std::vector<std::pair<ChessMove, std::string>> result;
            const auto all_moves = available_moves_ignoring_check();
            const auto moves = available_moves();
            for (const ChessMove &move : moves) {
                const ChessSquare src = move.source();
                const ChessSquare dst = move.destination();
                const ChessPiece piece = _board[src];
                const PieceType type = piece.type();
                std::ostringstream name;
                if (is_short_castle(move)) {
                    name << "O-O";
                } else if (is_long_castle(move)) {
                    name << "O-O-O";
                } else {
                    switch (type) {
                        case PieceType::NONE: {
                            throw std::invalid_argument(
                                "attempted to move from empty square"
                            );
                        }
                        case PieceType::KING  : { name << 'K'; break; }
                        case PieceType::QUEEN : { name << 'Q'; break; }
                        case PieceType::ROOK  : { name << 'R'; break; }
                        case PieceType::BISHOP: { name << 'B'; break; }
                        case PieceType::KNIGHT: { name << 'N'; break; }
                        case PieceType::PAWN: {
                            if (is_capture(move)) {
                                name << static_cast<char>('a' + src.file());
                            }
                            break;
                        }
                    }
                    if (type != PieceType::PAWN) {
                        bool ambiguous_rank = false;
                        bool ambiguous_file = false;
                        bool ambiguous_diag = false;
                        for (const ChessMove &other : all_moves) {
                            const ChessSquare osrc = other.source();
                            const ChessSquare odst = other.destination();
                            if ((_board[osrc].type() == type) && (odst == dst)) {
                                if ((osrc.rank() == src.rank()) &&
                                    (osrc.file() != src.file())) {
                                    ambiguous_rank = true;
                                } else if ((osrc.rank() != src.rank()) &&
                                           (osrc.file() == src.file())) {
                                    ambiguous_file = true;
                                } else if ((osrc.rank() != src.rank()) &&
                                           (osrc.file() != src.file())) {
                                    ambiguous_diag = true;
                                }
                            }
                        }
                        if (ambiguous_rank || ambiguous_file || ambiguous_diag) {
                            if (!ambiguous_file) {
                                name << static_cast<char>(MIN_FILE + src.file());
                            } else if (!ambiguous_rank) {
                                name << static_cast<char>(MIN_RANK + src.rank());
                            } else {
                                name << src;
                            }
                        }
                    }
                    if (is_capture(move)) { name << 'x'; }
                    name << dst;
                    if (move.promotion_type() != PieceType::NONE) {
                        name << '=';
                        switch (move.promotion_type()) {
                            case PieceType::NONE: {
                                throw std::invalid_argument("cannot promote to none");
                            }
                            case PieceType::KING: {
                                throw std::invalid_argument("cannot promote to king");
                            }
                            case PieceType::QUEEN : { name << 'Q'; break; }
                            case PieceType::ROOK  : { name << 'R'; break; }
                            case PieceType::BISHOP: { name << 'B'; break; }
                            case PieceType::KNIGHT: { name << 'N'; break; }
                            case PieceType::PAWN: {
                                throw std::invalid_argument("cannot promote to pawn");
                            }
                        }
                    }
                    if (puts_enemy_in_checkmate(move)) {
                        name << '#';
                    } else if (puts_enemy_in_stalemate(move)) {
                        name << '%';
                    } else if (puts_enemy_in_check(move)) {
                        name << '+';
                    }
                }
                result.emplace_back(move, name.str());
            }
            return result;
        }

    }; // class GameState


} // namespace DZChess


namespace std {
    template <>
    struct hash<DZChess::GameState> {
        size_t operator()(const DZChess::GameState &state) const {
            return state.board().hash();
        }
    }; // struct hash<DZChess::GameState>
} // namespace std


#endif // DZCHESS_GAME_STATE_HPP_INCLUDED
