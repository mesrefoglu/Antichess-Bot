#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>

#include "piece.h"

using namespace std;

#ifndef BOARD_H
#define BOARD_H

class Board
{
public:
  Piece square[64];
  int enPassantable = -1;
  // Initialize the board
  Board();

  // Makes move given input, no move validation
  // Coordinate algebraic notation, examples:
  // Normal move: e2e4
  // Pawn promotion: e7e8q
  // Castling as the King's two-square move: e1g1
  // En Passant, requires storage of last move: e5f6
  void makeMove(string move);

  string toAlgebraic(int square);

  vector<string> findPossibleMoves(uint8_t colour);

  // Print the board
  void print();
  
private:
  // "checkCheck" Design Philosophy:
  //    > should be called only by findPossibleMoves
  //    > Pass King's pos, to avoid recalculation
  //    > simulate the move
  //    > Only check necessary squares, in clockwise manner + Knight check
  // Removes moves that puts own king in check
  vector<string> checkCheck(uint8_t colour, vector<string> &moves, int kingPos);
  
};

#endif
