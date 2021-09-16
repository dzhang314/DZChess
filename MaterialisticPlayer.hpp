#ifndef DZCHESS_MATERIALISTIC_PLAYER_HPP_INCLUDED
#define DZCHESS_MATERIALISTIC_PLAYER_HPP_INCLUDED

#include <algorithm>     // for std::max
#include <cstddef>       // for std::size_t
#include <cstdio>        // TODO: REMOVE
#include <iostream>      // TODO: REMOVE
#include <limits>        // for std::numeric_limits::<T>infinity
#include <random>        // for std::uniform_int_distribution
#include <utility>       // for std::pair
#include <unordered_map> // for std::unordered_map

#include "ChessPiece.hpp"
#include "GameState.hpp"


namespace DZChess {


    class MaterialisticPlayer {

        std::mt19937 _rng;
        std::unordered_map<GameState, std::pair<int, double>> _transposition_table;
        std::size_t _hit_count;
        std::size_t _miss_count;
        std::size_t _reeval_count;

    public:

        explicit MaterialisticPlayer() :
            _rng(std::random_device{}()),
            _transposition_table(),
            _hit_count(0),
            _miss_count(0),
            _reeval_count(0) {}

        static constexpr double material_value(PieceType type) noexcept {
            switch (type) {
                case PieceType::NONE: return 0.0;
                case PieceType::KING: return 0.0;
                case PieceType::QUEEN: return 9.0;
                case PieceType::ROOK: return 5.0;
                case PieceType::BISHOP: return 3.0;
                case PieceType::KNIGHT: return 3.0;
                case PieceType::PAWN: return 1.0;
            }
        }

        constexpr double material_value(const GameState &state) const {
            const ChessBoard board = state.board();
            const PieceColor color_to_move = state.color_to_move();
            double result = 0.0;
            for (coord_t rank = 0; rank < BOARD_HEIGHT; ++rank) {
                for (coord_t file = 0; file < BOARD_WIDTH; ++file) {
                    const ChessSquare square{rank, file};
                    const ChessPiece piece = board[square];
                    if (piece.color() == color_to_move) {
                        result += material_value(piece.type());
                    } else {
                        result -= material_value(piece.type());
                    }
                }
            }
            return result;
        }

        double evaluate(const GameState &state, int depth) {
            if (depth <= 0) { return material_value(state); }
            if (_transposition_table.contains(state)) {
                const auto [eval_depth, score] = _transposition_table[state];
                if (eval_depth >= depth) {
                    ++_hit_count;
                    return score;
                } else {
                    ++_reeval_count;
                }
            } else {
                ++_miss_count;
            }
            double best_score = -std::numeric_limits<double>::infinity();
            for (const ChessMove &move : state.available_moves()) {
                const double score = -evaluate(state.after_move(move), depth - 1);
                best_score = std::max(best_score, score);
            }
            _transposition_table[state] = std::make_pair(depth, best_score);
            return best_score;
        }

        ChessMove select_move(const GameState &state) {
            double best_score = -std::numeric_limits<double>::infinity();
            std::vector<std::pair<ChessMove, std::string>> best_moves;
            for (const auto &[move, name] : state.available_moves_and_names()) {
                std::cout << "Considering " << name << "...\t";
                const double score = -evaluate(state.after_move(move), 4) + 0.0;
                std::printf("%+0.2f\n", score);
                if (score > best_score) {
                    best_score = score;
                    best_moves.clear();
                    best_moves.emplace_back(move, name);
                } else if (score == best_score) {
                    best_moves.emplace_back(move, name);
                }
            }
            std::cout << std::endl;
            std::uniform_int_distribution<std::size_t> dist(0, best_moves.size() - 1);
            const auto &[result, name] = best_moves[dist(_rng)];
            std::cout << "Selected move " << name << "." << std::endl;
            std::cout << "    Transposition table size: " << _transposition_table.size() << std::endl;
            std::cout << "    Hit count: " << _hit_count << std::endl;
            std::cout << "    Miss count: " << _miss_count << std::endl;
            std::cout << "    Re-eval count: " << _reeval_count << std::endl;
            std::cout << std::endl;
            _transposition_table.clear();
            _hit_count = 0;
            _miss_count = 0;
            _reeval_count = 0;
            return result;
        }

    }; // class MaterialisticPlayer


} // namespace DZChess

#endif // DZCHESS_MATERIALISTIC_PLAYER_HPP_INCLUDED
