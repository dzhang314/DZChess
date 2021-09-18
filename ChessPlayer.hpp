#ifndef DZCHESS_CHESS_PLAYER_HPP_INCLUDED
#define DZCHESS_CHESS_PLAYER_HPP_INCLUDED

#include <algorithm>     // for std::max
#include <compare>       // for operator<=>
#include <cstddef>       // for std::size_t
#include <cstdio>        // for std::printf
#include <iostream>      // for std::cin and std::cout
#include <limits>        // for std::numeric_limits
#include <random>        // for std::uniform_int_distribution
#include <stdexcept>     // for std::invalid_argument
#include <utility>       // for std::pair
#include <unordered_map> // for std::unordered_map

#include "ChessPiece.hpp"
#include "GameState.hpp"


namespace DZChess {


    class ChessPlayer {

    public:

        virtual ChessMove select_move(const GameState &state) = 0;

    }; // class ChessPlayer


    class ConsolePlayer : public ChessPlayer {

    public:

        virtual ChessMove select_move(const GameState &state) override {
            const auto moves = state.available_moves_and_names();
            if (state.in_check()) {
                std::cout << "You are in check. You have " << moves.size()
                    << " legal moves:" << std::endl;
                for (const auto &[move, name] : moves) {
                    std::cout << "    " << name << std::endl;
                }
                std::cout << std::endl;
            }
            while (true) {
                std::string selected_move;
                std::cin >> selected_move;
                std::cout << std::endl;
                int num_matches = 0;
                for (const auto &[move, name] : moves) {
                    if (name == selected_move) { ++num_matches; }
                }
                if (num_matches == 0) {
                    std::cout << selected_move << " is not a legal move. ";
                    std::cout << "The legal moves in this position are:" << std::endl;
                    for (const auto &[move, name] : moves) {
                        std::cout << "    " << name << std::endl;
                    }
                    std::cout << std::endl;
                } else if (num_matches == 1) {
                    for (const auto &[move, name] : moves) {
                        if (name == selected_move) { return move; }
                    }
                } else {
                    throw std::invalid_argument("move is ambiguous");
                }
            }
        }

    }; // class ConsolePlayer


    struct Evaluation {

        double _score;
        int _moves_until;

        constexpr auto operator<=>(const Evaluation &) const noexcept = default;

        explicit constexpr Evaluation() noexcept :
            _score(0.0), _moves_until(0) {
        }

        explicit constexpr Evaluation(double score) noexcept :
            _score(score), _moves_until(0) {
        }

        explicit constexpr Evaluation(double score, int moves_until) noexcept :
            _score(score), _moves_until(moves_until) {
        }

        constexpr Evaluation move_away() const noexcept {
            return Evaluation(-_score, _moves_until - 1);
        }

    }; // struct Evaluation


    class TreeSearchPlayer : public ChessPlayer {

    private:

        int _depth;
        bool _verbose;
        std::unordered_map<GameState, std::pair<int, Evaluation>> _transposition_table;
        std::size_t _hit_count;
        std::size_t _miss_count;
        std::size_t _reeval_count;
        std::mt19937 _rng;

    public:

        explicit TreeSearchPlayer(int depth, bool verbose = false) :
            _depth(depth),
            _verbose(verbose),
            _rng(std::random_device{}()),
            _transposition_table(),
            _hit_count(0),
            _miss_count(0),
            _reeval_count(0) {}

    protected:

        virtual Evaluation leaf_evaluation(const GameState &state) const = 0;

        Evaluation evaluate(const GameState &state, int depth) {
            if (depth <= 0) { return leaf_evaluation(state); }
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
            const auto moves = state.available_moves();
            if (moves.size() == 0) {
                if (state.in_check()) {
                    return Evaluation(-std::numeric_limits<double>::infinity(), 0);
                } else {
                    return Evaluation(0.0, 0);
                }
            }
            Evaluation best_score(-std::numeric_limits<double>::infinity(),
                                  std::numeric_limits<int>::min());
            for (const ChessMove &move : moves) {
                const Evaluation score = evaluate(state.after_move(move), depth - 1).move_away();
                best_score = std::max(best_score, score);
            }
            _transposition_table[state] = std::make_pair(depth, best_score);
            return best_score;
        }

    public:

        virtual ChessMove select_move(const GameState &state) override {
            Evaluation best_score(-std::numeric_limits<double>::infinity(),
                                  std::numeric_limits<int>::min());
            std::vector<std::pair<ChessMove, std::string>> best_moves;
            for (const auto &[move, name] : state.available_moves_and_names()) {
                if (_verbose) {
                    std::cout << "Considering " << name << "...\t";
                }
                const Evaluation score = evaluate(state.after_move(move),
                                                  _depth).move_away();
                if (_verbose) {
                    std::printf("(%+0.2f, %d)\n", score._score, score._moves_until);
                }
                if (score > best_score) {
                    best_score = score;
                    best_moves.clear();
                    best_moves.emplace_back(move, name);
                } else if (score == best_score) {
                    best_moves.emplace_back(move, name);
                }
            }
            if (_verbose) { std::cout << std::endl; }
            std::uniform_int_distribution<std::size_t> dist(0, best_moves.size() - 1);
            const auto &[result, name] = best_moves[dist(_rng)];
            if (_verbose) {
                std::cout << "Selected move " << name << "." << std::endl;
                std::cout << "    Transposition table size: ";
                std::cout << _transposition_table.size() << std::endl;
                std::cout << "    Hit count: " << _hit_count << std::endl;
                std::cout << "    Miss count: " << _miss_count << std::endl;
                std::cout << "    Re-eval count: " << _reeval_count << std::endl;
                std::cout << std::endl;
            }
            _transposition_table.clear();
            _hit_count = 0;
            _miss_count = 0;
            _reeval_count = 0;
            return result;
        }

    }; // class TreeSearchPlayer


    constexpr double material_value(PieceType type) noexcept {
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


    constexpr double material_value(const GameState &state) {
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


    class MaterialisticPlayer : public TreeSearchPlayer {

    public:

        explicit MaterialisticPlayer(int depth, bool verbose = false) :
            TreeSearchPlayer(depth, verbose) {}

        virtual constexpr Evaluation
        leaf_evaluation(const GameState &state) const override {
            return Evaluation(material_value(state));
        }

    }; // class MaterialisticPlayer


    class MovesPlayer : public TreeSearchPlayer {

    public:

        explicit MovesPlayer(int depth, bool verbose = false) :
            TreeSearchPlayer(depth, verbose) {}

        virtual constexpr Evaluation
        leaf_evaluation(const GameState &state) const override {
            return Evaluation(material_value(state)
                              + 0.01 * state.available_moves().size());
        }

    }; // class MaterialisticPlayer


} // namespace DZChess


#endif // DZCHESS_CHESS_PLAYER_HPP_INCLUDED
