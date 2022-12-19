#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>

using namespace std;

#ifndef PIECE_H
#define PIECE_H

// A Piece is represented as 8-bits
// Leftmost 2-bits represent color
// Rightmost 6-bits represent type of piece
class Piece
{
public:
  uint8_t x;
  bool hasMoved = false;

  Piece() : x(0){};
  Piece(uint8_t x) : x(x){};

  const static uint8_t None = 0;
  const static uint8_t Pawn = 1;
  const static uint8_t Bishop = 2;
  const static uint8_t Knight = 4;
  const static uint8_t Rook = 8;
  const static uint8_t Queen = 16;
  const static uint8_t King = 32;
  const static uint8_t White = 64;
  const static uint8_t Black = 128;
};

#endif
