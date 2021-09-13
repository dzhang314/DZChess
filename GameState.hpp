#ifndef DZCHESS_GAME_STATE_HPP_INCLUDED
#define DZCHESS_GAME_STATE_HPP_INCLUDED

#include <vector>

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

        constexpr coord_t pawn_origin_rank() const {
            return (_color_to_move == PieceColor::WHITE) ? 1 : 6;
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
        ) const {
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
        ) const {
            ChessSquare destination = source + displacement;
            if (can_move_to(destination)) {
                moves.emplace_back(source, destination);
            }
        }

        constexpr void available_moves(
            std::vector<ChessMove> &moves,
            const ChessSquare &source
        ) const {
            const ChessPiece piece = _board[source];
            if (piece.color() == _color_to_move) {
                switch (piece.type()) {
                    case PieceType::NONE: {
                        break;
                    }
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
                        const coord_t forward =
                            (_color_to_move == PieceColor::WHITE) ? +1 : -1;
                        const ChessDisplacement one{forward, 0};
                        if (is_empty(source + one)) {
                            moves.emplace_back(source, source + one);
                            if ((source.rank() == pawn_origin_rank())
                                && is_empty(source + one + one)) {
                                moves.emplace_back(source, source + one + one);
                            }
                        }
                        const ChessDisplacement ldiag{forward, -1};
                        if (has_enemy_piece(source + ldiag)) {
                            moves.emplace_back(source, source + ldiag);
                        }
                        const ChessDisplacement rdiag{forward, +1};
                        if (has_enemy_piece(source + rdiag)) {
                            moves.emplace_back(source, source + rdiag);
                        }
                        break;
                    }
                }
            }
        }

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
