cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

project("DZChess")

add_executable(DZChess "ChessPiece.hpp" "ChessBoard.hpp" "GameState.hpp" "ChessPlayer.hpp" "DZChess.cpp")
