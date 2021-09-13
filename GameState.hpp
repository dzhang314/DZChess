#ifndef DZCHESS_GAME_STATE_HPP_INCLUDED
#define DZCHESS_GAME_STATE_HPP_INCLUDED

#include <sstream> // for std::ostringstream
#include <string>  // for std::string
#include <utility> // for std::pair
#include <vector>  // for std::vector

#include "ChessPiece.hpp"
#include "ChessBoard.hpp"


namespace DZChess {


    class GameState {

    private:

        ChessBoard _board;
        ChessMove _last_move;
        PieceColor _color_to_move;
        bool _white_short_castle_available;
        bool _white_long_castle_available;
        bool _black_short_castle_available;
        bool _black_long_castle_available;

    public:

        explicit constexpr GameState() noexcept :
            _board(),                            // starting position
            _last_move(),                        // null last move
            _color_to_move(PieceColor::WHITE),   // white goes first
            _white_short_castle_available(true), // all castling available
            _white_long_castle_available(true),
            _black_short_castle_available(true),
            _black_long_castle_available(true) {}

        constexpr const ChessBoard &board    () const noexcept { return _board    ; }
        constexpr const ChessMove  &last_move() const noexcept { return _last_move; }

    private:

        constexpr coord_t pawn_origin_rank() const noexcept {
            return (_color_to_move == PieceColor::WHITE) ? 1 : (BOARD_HEIGHT - 2);
        }

        constexpr bool is_empty(ChessSquare square) const noexcept {
            return square.in_bounds() && (_board[square].color() == PieceColor::NONE);
        }

        constexpr bool has_own_piece(ChessSquare square) const noexcept {
            return square.in_bounds() && (_board[square].color() == _color_to_move);
        }

        constexpr bool has_enemy_piece(ChessSquare square) const noexcept {
            if (!square.in_bounds()) { return false; }
            const PieceColor color = _board[square].color();
            return (color != PieceColor::NONE) && (color != _color_to_move);
        }

        constexpr bool can_move_to(ChessSquare square) const noexcept {
            return square.in_bounds() && (_board[square].color() != _color_to_move);
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

        constexpr bool is_capture(const ChessMove &move) const {
            return has_enemy_piece(move.destination());
        }

        constexpr void promotion_moves(
            std::vector<ChessMove> &moves,
            ChessSquare source,
            ChessSquare destination
        ) const noexcept {
            if ((destination.rank() == 0) ||
                (destination.rank() == BOARD_HEIGHT - 1)) {
                moves.emplace_back(source, destination, PieceType::QUEEN );
                moves.emplace_back(source, destination, PieceType::ROOK  );
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
            const coord_t up =
                (_color_to_move == PieceColor::WHITE) ? +1 : -1;
            const ChessSquare one = source + ChessDisplacement{up, 0};
            if (is_empty(one)) {
                promotion_moves(moves, source, one);
                const ChessSquare two = one + ChessDisplacement{up, 0};
                if ((source.rank() == pawn_origin_rank()) && is_empty(two)) {
                    promotion_moves(moves, source, two);
                }
            }
            const ChessSquare ldiag = source + ChessDisplacement{up, -1};
            if (has_enemy_piece(ldiag)) {
                promotion_moves(moves, source, ldiag);
            }
            const ChessSquare rdiag = source + ChessDisplacement{up, +1};
            if (has_enemy_piece(rdiag)) {
                promotion_moves(moves, source, rdiag);
            }
        }

        constexpr void available_moves(
            std::vector<ChessMove> &moves,
            const ChessSquare &source
        ) const {
            const ChessPiece piece = _board[source];
            if (piece.color() == _color_to_move) {
                switch (piece.type()) {
                    case PieceType::NONE: { break; }
                    case PieceType::KING: {
                        leaper_move(moves, source, {+1,  0});
                        leaper_move(moves, source, {-1,  0});
                        leaper_move(moves, source, { 0, +1});
                        leaper_move(moves, source, { 0, -1});
                        leaper_move(moves, source, {+1, +1});
                        leaper_move(moves, source, {+1, -1});
                        leaper_move(moves, source, {-1, +1});
                        leaper_move(moves, source, {-1, -1});
                        break;
                    }
                    case PieceType::QUEEN: {
                        slider_moves(moves, source, {+1,  0});
                        slider_moves(moves, source, {-1,  0});
                        slider_moves(moves, source, { 0, +1});
                        slider_moves(moves, source, { 0, -1});
                        slider_moves(moves, source, {+1, +1});
                        slider_moves(moves, source, {+1, -1});
                        slider_moves(moves, source, {-1, +1});
                        slider_moves(moves, source, {-1, -1});
                        break;
                    }
                    case PieceType::ROOK: {
                        slider_moves(moves, source, {+1,  0});
                        slider_moves(moves, source, {-1,  0});
                        slider_moves(moves, source, { 0, +1});
                        slider_moves(moves, source, { 0, -1});
                        break;
                    }
                    case PieceType::BISHOP: {
                        slider_moves(moves, source, {+1, +1});
                        slider_moves(moves, source, {+1, -1});
                        slider_moves(moves, source, {-1, +1});
                        slider_moves(moves, source, {-1, -1});
                        break;
                    }
                    case PieceType::KNIGHT: {
                        leaper_move(moves, source, {+1, +2});
                        leaper_move(moves, source, {+1, -2});
                        leaper_move(moves, source, {-1, +2});
                        leaper_move(moves, source, {-1, -2});
                        leaper_move(moves, source, {+2, +1});
                        leaper_move(moves, source, {+2, -1});
                        leaper_move(moves, source, {-2, +1});
                        leaper_move(moves, source, {-2, -1});
                        break;
                    }
                    case PieceType::PAWN: {
                        pawn_moves(moves, source);
                        break;
                    }
                }
            }
        }

    public:

        constexpr std::vector<ChessMove> available_moves() const noexcept {
            std::vector<ChessMove> result;
            for (coord_t rank = 0; rank < BOARD_HEIGHT; ++rank) {
                for (coord_t file = 0; file < BOARD_WIDTH; ++file) {
                    const ChessSquare source{rank, file};
                    available_moves(result, source);
                }
            }
            return result;
        }

        std::vector<std::pair<ChessMove, std::string>>
        available_moves_and_names() const {
            std::vector<std::pair<ChessMove, std::string>> result;
            const auto moves = available_moves();
            for (const ChessMove &move : moves) {
                std::ostringstream name;
                if (is_capture(move)) {
                    switch (_board[move.source()].type()) {
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
                            name << static_cast<char>('a' + move.source().file());
                            break;
                        }
                    }
                    name << 'x' << move.destination();
                    result.emplace_back(move, name.str());
                } else {
                    switch (_board[move.source()].type()) {
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
                        case PieceType::PAWN  : {              break; }
                    }
                    name << move.destination();
                    result.emplace_back(move, name.str());
                }
            }
            return result;
        }

        void make_move(const ChessMove &move) {
            if (!has_own_piece(move.source())) {
                throw std::invalid_argument("attempted to move invalid piece");
            }
            if (!can_move_to(move.destination())) {
                throw std::invalid_argument("attempted to move to invalid square");
            }
            _board.make_move(move);
            _last_move = move;
            _color_to_move = (_color_to_move == PieceColor::BLACK)
                             ? PieceColor::WHITE : PieceColor::BLACK;
        }

    }; // class GameState


} // namespace DZChess


#endif // DZCHESS_GAME_STATE_HPP_INCLUDED
