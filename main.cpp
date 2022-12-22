#include <iostream>
#include <string>
#include <vector>
#include <bitset>
using namespace std;
#ifndef DEPTH
#define DEPTH 5
#endif

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

class Board
{
public:
  Piece square[64];
  int enPassantable = -1;
  bool castleableBQ = true;
  bool castleableBK = true;
  bool castleableWQ = true;
  bool castleableWK = true;
  // Initialize the board
  Board()
  {
    // Set up the starting position
    square[0] = Piece(Piece::Rook | Piece::Black);
    square[1] = Piece(Piece::Knight | Piece::Black);
    square[2] = Piece(Piece::Bishop | Piece::Black);
    square[3] = Piece(Piece::Queen | Piece::Black);
    square[4] = Piece(Piece::King | Piece::Black);
    square[5] = Piece(Piece::Bishop | Piece::Black);
    square[6] = Piece(Piece::Knight | Piece::Black);
    square[7] = Piece(Piece::Rook | Piece::Black);
    for (int i = 8; i < 16; i++)
      square[i] = Piece(Piece::Pawn | Piece::Black);
    for (int i = 16; i < 48; i++)
      square[i] = Piece(Piece::None);
    for (int i = 48; i < 56; i++)
      square[i] = Piece(Piece::Pawn | Piece::White);
    square[56] = Piece(Piece::Rook | Piece::White);
    square[57] = Piece(Piece::Knight | Piece::White);
    square[58] = Piece(Piece::Bishop | Piece::White);
    square[59] = Piece(Piece::Queen | Piece::White);
    square[60] = Piece(Piece::King | Piece::White);
    square[61] = Piece(Piece::Bishop | Piece::White);
    square[62] = Piece(Piece::Knight | Piece::White);
    square[63] = Piece(Piece::Rook | Piece::White);
  }

  // Makes move given input, no move validation
  void makeMove(string move)
  {
    int from = (move[0] - 'a') + 56 - (move[1] - '1') * 8;
    int to = (move[2] - 'a') + 56 - (move[3] - '1') * 8;
    square[to].x = square[from].x;
    square[to].hasMoved = true;
    square[from] = Piece(Piece::None);
    // Reset en passantable flag
    if (enPassantable != to)
      enPassantable = -1;
    // Promotion Case:
    if (move.length() == 5)
    {
      if (move[4] == 'q')
        square[to].x = Piece::Queen | (square[to].x & Piece::White ? Piece::White : Piece::Black);
      else if (move[4] == 'r')
        square[to].x = Piece::Rook | (square[to].x & Piece::White ? Piece::White : Piece::Black);
      else if (move[4] == 'b')
        square[to].x = Piece::Bishop | (square[to].x & Piece::White ? Piece::White : Piece::Black);
      else if (move[4] == 'n')
        square[to].x = Piece::Knight | (square[to].x & Piece::White ? Piece::White : Piece::Black);
    }
    // Castling Case:
    else if (square[to].x == (Piece::King | Piece::Black))
    {
      if (to == 2 && castleableBQ)
      {
        square[0] = Piece(Piece::None);
        square[3] = Piece(Piece::Rook | Piece::Black);
      }
      else if (to == 6 && castleableBK)
      {
        square[7] = Piece(Piece::None);
        square[5] = Piece(Piece::Rook | Piece::Black);
      }

      castleableBQ = false;
      castleableBK = false;
    }
    else if (square[to].x == (Piece::King | Piece::White))
    {
      if (to == 58 && castleableWQ)
      {
        square[56] = Piece(Piece::None);
        square[59] = Piece(Piece::Rook | Piece::White);
      }
      else if (to == 62 && castleableWK)
      {
        square[63] = Piece(Piece::None);
        square[61] = Piece(Piece::Rook | Piece::White);
      }

      castleableWQ = false;
      castleableWK = false;
    }
    // En Passant Case:
    else if ((square[to].x & 63) == Piece::Pawn)
    {
      if (to == from + 16)
        enPassantable = from + 8;
      else if (to == from - 16)
        enPassantable = from - 8;
      else if ((to == from + 7 || to == from + 9) && enPassantable == to)
      {
        square[to - 8] = Piece(Piece::None);
        enPassantable = -1;
      }
      else if ((to == from - 7 || to == from - 9) && enPassantable == to)
      {
        square[to + 8] = Piece(Piece::None);
        enPassantable = -1;
      }
    }
    if (castleableBQ && !(square[0].x == (Piece::Rook | Piece::Black)))
      castleableBQ = false;
    if (castleableBK && !(square[7].x == (Piece::Rook | Piece::Black)))
      castleableBK = false;
    if (castleableWQ && !(square[56].x == (Piece::Rook | Piece::White)))
      castleableWQ = false;
    if (castleableWK && !(square[63].x == (Piece::Rook | Piece::White)))
      castleableWK = false;
  }

  // Returns a string representation of a square
  string toAlgebraic(int square)
  {
    string result;
    result += (char)('a' + square % 8);
    result += (char)('1' + 7 - square / 8);
    return result;
  }

  // Returns a vector of all possible moves for a given colour
  vector<string> findPossibleMoves(uint8_t colour)
  {
    // moves has priority over secondary
    // secondary only used if checked at start of term
    // secondary stores non-take moves in this case
    vector<string> moves;
    vector<string> secondary;
    int kingPos = kingFind(colour);
    bool checked = inCheck(colour, toAlgebraic(kingPos) + toAlgebraic(kingPos), kingPos);
    uint8_t opposite = colour == Piece::White ? Piece::Black : Piece::White;
    for (int i = 0; i < 64; i++)
    {
      if (!(square[i].x & colour))
        continue;
      uint8_t piece = square[i].x & 63;
      if (piece == Piece::Bishop || piece == Piece::Queen)
      {
        for (int j = 1; i + j * 9 < 64 && i % 8 + j < 8; j++)
        {
          if (square[i + j * 9].x & colour)
            break;
          if (square[i + j * 9].x & opposite)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 9));
            break;
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + j * 9));
        }
        for (int j = 1; i + j * 7 < 64 && i % 8 - j >= 0; j++)
        {
          if (square[i + j * 7].x & colour)
            break;
          if (square[i + j * 7].x & opposite)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 7));
            break;
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + j * 7));
        }
        for (int j = 1; i - j * 9 >= 0 && i % 8 - j >= 0; j++)
        {
          if (square[i - j * 9].x & colour)
            break;
          if (square[i - j * 9].x & opposite)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 9));
            break;
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - j * 9));
        }
        for (int j = 1; i - j * 7 >= 0 && i % 8 + j < 8; j++)
        {
          if (square[i - j * 7].x & colour)
            break;
          if (square[i - j * 7].x & opposite)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 7));
            break;
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - j * 7));
        }
      }
      if (piece == Piece::Rook || piece == Piece::Queen)
      {
        for (int j = 1; i + j < 64 && i % 8 + j < 8; j++)
        {
          if (square[i + j].x & colour)
            break;
          if (square[i + j].x & opposite)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j));
            break;
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + j));
        }
        for (int j = 1; i - j >= 0 && i % 8 - j >= 0; j++)
        {
          if (square[i - j].x & colour)
            break;
          if (square[i - j].x & opposite)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j));
            break;
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - j));
        }
        for (int j = 1; i + j * 8 < 64; j++)
        {
          if (square[i + j * 8].x & colour)
            break;
          if (square[i + j * 8].x & opposite)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 8));
            break;
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + j * 8));
        }
        for (int j = 1; i - j * 8 >= 0; j++)
        {
          if (square[i - j * 8].x & colour)
            break;
          if (square[i - j * 8].x & opposite)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 8));
            break;
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - j * 8));
        }
      }
      else if (piece == Piece::Pawn)
      {
        if (colour == Piece::White)
        {
          if (i % 8 != 7 && (square[i - 7].x & opposite || enPassantable == i - 7))
          {

            // Check for promotion
            if (i - 7 < 8)
              moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7) + 'q');
            else
              moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7));
          }
          if (i % 8 != 0 && (square[i - 9].x & opposite || enPassantable == i - 9))
          {

            // Check for promotion
            if (i - 9 < 8)
              moves.push_back(toAlgebraic(i) + toAlgebraic(i - 9) + 'q');
            else
              moves.push_back(toAlgebraic(i) + toAlgebraic(i - 9));
          }
          if (square[i - 8].x == Piece::None)
          {
            // Check for promotion
            if (i - 8 < 8)
              secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 8) + 'q');
            else
              secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
          }
          if (!square[i].hasMoved && square[i - 8].x == Piece::None && square[i - 16].x == Piece::None)
          {
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 16));
          }
        }
        else
        {
          if (i % 8 != 0 && (square[i + 7].x & opposite || enPassantable == i + 7))
          {

            // Check for promotion
            if (i + 7 >= 56)
              moves.push_back(toAlgebraic(i) + toAlgebraic(i + 7) + 'q');
            else
              moves.push_back(toAlgebraic(i) + toAlgebraic(i + 7));
          }
          if (i % 8 != 7 && (square[i + 9].x & opposite || enPassantable == i + 9))
          {
            // Check for promotion
            if (i + 9 >= 56)
              moves.push_back(toAlgebraic(i) + toAlgebraic(i + 9) + 'q');
            else
              moves.push_back(toAlgebraic(i) + toAlgebraic(i + 9));
          }

          if (square[i + 8].x == Piece::None)
          {
            // Check for promotion
            if (i + 8 >= 56)
              secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 8) + 'q');
            else
              secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
          }

          if (!square[i].hasMoved && square[i + 8].x == Piece::None && square[i + 16].x == Piece::None)
          {
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 16));
          }
        }
      }
      else if (piece == Piece::Knight)
      {
        if (i + 17 < 64 && i % 8 <= 6 && !(square[i + 17].x & colour))
        {
          if (square[i + 17].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 17));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 17));
        }
        if (i + 15 < 64 && i % 8 >= 1 && !(square[i + 15].x & colour))
        {
          if (square[i + 15].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 15));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 15));
        }
        if (i + 10 < 64 && i % 8 <= 5 && !(square[i + 10].x & colour))
        {
          if (square[i + 10].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 10));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 10));
        }
        if (i + 6 < 64 && i % 8 >= 2 && !(square[i + 6].x & colour))
        {
          if (square[i + 6].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 6));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 6));
        }
        if (i - 17 >= 0 && i % 8 >= 1 && !(square[i - 17].x & colour))
        {
          if (square[i - 17].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 17));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 17));
        }
        if (i - 15 >= 0 && i % 8 <= 6 && !(square[i - 15].x & colour))
        {
          if (square[i - 15].x & opposite)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 15));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 15));
        }
        if (i - 10 >= 0 && i % 8 >= 2 && !(square[i - 10].x & colour))
        {
          if (square[i - 10].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 10));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 10));
        }
        if (i - 6 >= 0 && i % 8 <= 5 && !(square[i - 6].x & colour))
        {
          if (square[i - 6].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 6));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 6));
        }
      }
      else if (piece == Piece::King)
      {
        if (i + 1 < 64 && i % 8 + 1 < 8 && !(square[i + 1].x & colour))
        {
          if (square[i + 1].x & opposite)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 1));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 1));
        }
        if (i - 1 >= 0 && i % 8 - 1 >= 0 && !(square[i - 1].x & colour))
        {
          if (square[i - 1].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 1));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 1));
        }
        if (i + 8 < 64 && !(square[i + 8].x & colour))
        {
          if (square[i + 8].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
        }
        if (i - 8 >= 0 && !(square[i - 8].x & colour))
        {
          if (square[i - 8].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
        }
        if (i + 9 < 64 && i % 8 + 1 < 8 && !(square[i + 9].x & colour))
        {
          if (square[i + 9].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 9));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 9));
        }
        if (i - 9 >= 0 && i % 8 - 1 >= 0 && !(square[i - 9].x & colour))
        {
          if (square[i - 9].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 9));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 9));
        }
        if (i + 7 < 64 && i % 8 - 1 >= 0 && !(square[i + 7].x & colour))
        {
          if (square[i + 7].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 7));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 7));
        }
        if (i - 7 >= 0 && i % 8 + 1 < 8 && !(square[i - 7].x & colour))
        {
          if (square[i - 7].x & opposite)
          {

            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7));
          }
          else
            secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 7));
        }
        // Castling
        if (!checked)
        {
          // Queenside Black Castle
          if (castleableBQ && (square[3].x == Piece::None) && (square[2].x == Piece::None) && (square[1].x == Piece::None))
          {
            // Check if any of path to King's final spot threatened
            // Final spot itself checked in final checkCheck
            if (!inCheck(colour, toAlgebraic(4) + toAlgebraic(3), kingPos))
            {
              secondary.push_back(toAlgebraic(4) + toAlgebraic(2));
            }
          }
          // Kingside Black Castle
          if (castleableBK && (square[5].x == Piece::None) && (square[6].x == Piece::None))
          {
            // Check if any of path to King's final spot threatened
            // Final spot itself checked in final checkCheck
            if (!inCheck(colour, toAlgebraic(4) + toAlgebraic(5), kingPos))
            {
              secondary.push_back(toAlgebraic(4) + toAlgebraic(6));
            }
          }
          // Queenside White Castle
          if (castleableWQ && (square[59].x == Piece::None) && (square[58].x == Piece::None) && (square[57].x == Piece::None))
          {
            // Check if any of path to King's final spot threatened
            // Final spot itself checked in final checkCheck
            if (!inCheck(colour, toAlgebraic(60) + toAlgebraic(59), kingPos))
            {
              secondary.push_back(toAlgebraic(60) + toAlgebraic(58));
            }
          }
          // Kingside White Castle
          if (castleableWK && (square[61].x == Piece::None) && (square[62].x == Piece::None))
          {
            // Check if any of path to King's final spot threatened
            // Final spot itself checked in final checkCheck
            if (!inCheck(colour, toAlgebraic(60) + toAlgebraic(61), kingPos))
            {
              secondary.push_back(toAlgebraic(60) + toAlgebraic(62));
            }
          }
        }
      }
    }
    // Check Move validation
    vector<string> checkedMoves;
    for (auto move : moves)
    {
      if (inCheck(colour, move, kingPos) == false)
        checkedMoves.emplace_back(move);
    }

    // Check if we need to go to secondary moves
    if (checkedMoves.size() == 0)
    {
      for (auto move : secondary)
      {
        if (inCheck(colour, move, kingPos) == false)
          checkedMoves.emplace_back(move);
      }
    }

    return (checkedMoves);
  }

  //// Returns true if the king of the given colour is in check
  // bool inCheck(uint8_t colour, int i = -1) // i is the index of the king
  //{
  //   uint8_t opposite = colour == Piece::White ? Piece::Black : Piece::White;
  //   // Look around the king to see any threats
  //   if (i == -1)
  //   {
  //     for (int j = 0; j < 64; j++)
  //     {
  //       if (square[i].x == (Piece::King | colour))
  //       {
  //         i = j;
  //         break;
  //       }
  //     }
  //   }
  //   // Check for pawns
  //   if (colour == Piece::White)
  //   {
  //     if (i - 7 >= 0 && i % 8 + 1 < 8 && square[i - 7].x == (Piece::Pawn | opposite))
  //       return true;
  //     if (i - 9 >= 0 && i % 8 - 1 >= 0 && square[i - 9].x == (Piece::Pawn | opposite))
  //       return true;
  //   }
  //   else
  //   {
  //     if (i + 7 >= 0 && i % 8 - 1 >= 0 && square[i + 7].x == (Piece::Pawn | opposite))
  //       return true;
  //     if (i + 9 >= 0 && i % 8 + 1 < 8 && square[i + 9].x == (Piece::Pawn | opposite))
  //       return true;
  //   }
  //   // Check for knights
  //   if (i - 15 >= 0 && i % 8 + 1 < 8 && square[i - 15].x == (Piece::Knight | opposite))
  //     return true;
  //   if (i - 6 >= 0 && i % 8 + 2 < 8 && square[i - 6].x == (Piece::Knight | opposite))
  //     return true;
  //   if (i + 10 >= 0 && i % 8 + 2 < 8 && square[i + 10].x == (Piece::Knight | opposite))
  //     return true;
  //   if (i + 17 >= 0 && i % 8 + 1 < 8 && square[i + 17].x == (Piece::Knight | opposite))
  //     return true;
  //   if (i + 15 >= 0 && i % 8 - 1 >= 0 && square[i + 15].x == (Piece::Knight | opposite))
  //     return true;
  //   if (i + 6 >= 0 && i % 8 - 2 >= 0 && square[i + 6].x == (Piece::Knight | opposite))
  //     return true;
  //   if (i - 10 >= 0 && i % 8 - 2 >= 0 && square[i - 10].x == (Piece::Knight | opposite))
  //     return true;
  //   if (i - 17 >= 0 && i % 8 - 1 >= 0 && square[i - 17].x == (Piece::Knight | opposite))
  //     return true;
  //   // Check for rooks and queens
  //   // North
  //   for (int j = i - 8; j >= 0; j -= 8)
  //   {
  //     if (square[j].x == (Piece::Rook | opposite) || square[j].x == (Piece::Queen | opposite))
  //       return true;
  //     if (square[j].x != Piece::None)
  //       break;
  //   }
  //   // East
  //   for (int j = i + 1; j % 8 != 0; j++)
  //   {
  //     if (square[j].x == (Piece::Rook | opposite) || square[j].x == (Piece::Queen | opposite))
  //       return true;
  //     if (square[j].x != Piece::None)
  //       break;
  //   }
  //   // South
  //   for (int j = i + 8; j < 64; j += 8)
  //   {
  //     if (square[j].x == (Piece::Rook | opposite) || square[j].x == (Piece::Queen | opposite))
  //       return true;
  //     if (square[j].x != Piece::None)
  //       break;
  //   }
  //   // West
  //   for (int j = i - 1; j % 8 != 7; j--)
  //   {
  //     if (square[j].x == (Piece::Rook | opposite) || square[j].x == (Piece::Queen | opposite))
  //       return true;
  //     if (square[j].x != Piece::None)
  //       break;
  //   }
  //   // Check for bishops and queens
  //   // North East
  //   for (int j = i - 7; j >= 0 && j % 8 != 0; j -= 7)
  //   {
  //     if (square[j].x == (Piece::Bishop | opposite) || square[j].x == (Piece::Queen | opposite))
  //       return true;
  //     if (square[j].x != Piece::None)
  //       break;
  //   }
  //   // South East
  //   for (int j = i + 9; j < 64 && j % 8 != 0; j += 9)
  //   {
  //     if (square[j].x == (Piece::Bishop | opposite) || square[j].x == (Piece::Queen | opposite))
  //       return true;
  //     if (square[j].x != Piece::None)
  //       break;
  //   }
  //   // South West
  //   for (int j = i + 7; j < 64 && j % 8 != 7; j += 7)
  //   {
  //     if (square[j].x == (Piece::Bishop | opposite) || square[j].x == (Piece::Queen | opposite))
  //       return true;
  //     if (square[j].x != Piece::None)
  //       break;
  //   }
  //   // North West
  //   for (int j = i - 9; j >= 0 && j % 8 != 7; j -= 9)
  //   {
  //     if (square[j].x == (Piece::Bishop | opposite) || square[j].x == (Piece::Queen | opposite))
  //       return true;
  //     if (square[j].x != Piece::None)
  //       break;
  //   }
  //   return false;
  // }

  bool inCheck(uint8_t colour, string move, int kingPos)
  {
    uint8_t opposite = (colour == Piece::White ? Piece::Black : Piece::White);
    // Treat 'to' as impassable
    //    'from' as empty
    int from = (move[0] - 'a') + 56 - (move[1] - '1') * 8;
    int to = (move[2] - 'a') + 56 - (move[3] - '1') * 8;

    int curKingPos = kingPos;
    // Change curKingPos if King was the piece which moved
    if (from == kingPos)
      curKingPos = to;
    // Clockwise rotation around King
    for (int i = curKingPos - 8; i >= 0; i -= 8)
    {
      // treat 'from' as empty
      if (square[i].x == 0 || i == from)
        continue;
      // treat 'to' as own piece
      else if (square[i].x & colour || i == to)
        break;
      else if (square[i].x & Piece::Queen || square[i].x & Piece::Rook)
      {
        return (true);
      }
      else if (square[i].x & Piece::King && curKingPos - i <= 8)
      {
        return (true);
      }
      else
        break;
    }
    // Check for Knights
    if ((curKingPos - 17 >= 0) && (curKingPos - 17 % 8 != 7) && square[curKingPos - 17].x == (opposite | Piece::Knight) && curKingPos - 17 != to)
    {
      return (true);
    }
    else if ((curKingPos - 15 >= 0) && (curKingPos - 15 % 8 != 0) && square[curKingPos - 15].x == (opposite | Piece::Knight) && curKingPos - 15 != to)
    {
      return (true);
    }
    else if ((curKingPos - 10 >= 0) && (curKingPos - 10 % 8 != 6) && (curKingPos - 10 % 8 != 7) && square[curKingPos - 10].x == (opposite | Piece::Knight) && curKingPos - 10 != to)
    {
      return (true);
    }
    else if ((curKingPos - 6 >= 0) && (curKingPos - 6 % 8 != 0) && (curKingPos - 6 % 8 != 1) && square[curKingPos - 6].x == (opposite | Piece::Knight) && curKingPos - 6 != to)
    {
      return (true);
    }
    else if ((curKingPos + 6 <= 63) && (curKingPos + 6 % 8 != 6) && (curKingPos - 10 % 8 != 7) && square[curKingPos + 6].x == (opposite | Piece::Knight) && curKingPos - 10 != to)
    {
      return (true);
    }
    else if ((curKingPos + 10 <= 63) && (curKingPos + 10 % 8 != 0) && (curKingPos + 10 % 8 != 1) && square[curKingPos + 10].x == (opposite | Piece::Knight) && curKingPos + 10 != to)
    {
      return (true);
    }
    else if ((curKingPos + 15 <= 63) && (curKingPos + 15 % 8 != 7) && square[curKingPos + 15].x == (opposite | Piece::Knight) && curKingPos + 15 != to)
    {
      return (true);
    }
    else if ((curKingPos + 17 <= 63) && (curKingPos + 17 % 8 != 0) && square[curKingPos + 17].x == (opposite | Piece::Knight) && curKingPos + 17 != to)
    {
      return (true);
    }
    // NE, E, SE, S, SW, W, NW
    for (int i = curKingPos - 7; i >= 0 && i % 8 != 0; i -= 7)
    {
      if (square[i].x == 0 || i == from)
        continue;
      else if (square[i].x & colour || i == to)
        break;
      else if (square[i].x & Piece::Queen || square[i].x & Piece::Bishop)
      {
        return (true);
      }
      else if (square[i].x & Piece::King && curKingPos - i <= 7)
      {
        return (true);
      }
      else if (square[i].x & Piece::Pawn && curKingPos - i <= 8 && colour & Piece::White)
      {
        return (true);
      }
      else
        break;
    }
    for (int i = curKingPos + 1; i % 8 != 0; i++)
    {
      if (square[i].x == 0 || i == from)
        continue;
      else if (square[i].x & colour || i == to)
        break;
      else if (square[i].x & Piece::Queen || square[i].x & Piece::Rook)
      {
        return (true);
      }
      else if (square[i].x & Piece::King && i - curKingPos <= 1)
      {
        return (true);
      }
      else
        break;
    }

    for (int i = curKingPos + 9; i <= 63 && i % 8 != 0; i += 9)
    {
      if (square[i].x == 0 || i == from)
        continue;
      else if (square[i].x & colour || i == to)
        break;
      else if (square[i].x & Piece::Queen || square[i].x & Piece::Bishop)
      {
        return (true);
      }
      else if (square[i].x & Piece::King && i - curKingPos <= 9)
      {
        return (true);
      }
      else if (square[i].x & Piece::Pawn && i - curKingPos <= 9 && colour & Piece::Black)
      {
        return (true);
      }
      else
        break;
    }

    for (int i = curKingPos + 8; i <= 63; i += 8)
    {
      if (square[i].x == 0 || i == from)
        continue;
      else if (square[i].x & colour || i == to)
        break;
      else if (square[i].x & Piece::Queen || square[i].x & Piece::Rook)
      {
        return (true);
      }
      else if (square[i].x & Piece::King && i - curKingPos <= 8)
      {
        return (true);
      }
      else
        break;
    }

    for (int i = curKingPos + 7; i <= 63 && i % 8 != 7; i += 7)
    {
      if (square[i].x == 0 || i == from)
        continue;
      else if (square[i].x & colour || i == to)
        break;
      else if (square[i].x & Piece::Queen || square[i].x & Piece::Bishop)
      {
        return (true);
      }
      else if (square[i].x & Piece::King && i - curKingPos <= 7)
      {
        return (true);
      }
      else if (square[i].x & Piece::Pawn && i - curKingPos <= 8 && colour & Piece::Black)
      {
        return (true);
      }
      else
        break;
    }

    for (int i = curKingPos - 1; i % 8 != 7; i--)
    {
      if (square[i].x == 0 || i == from)
        continue;
      else if (square[i].x & colour || i == to)
        break;
      else if (square[i].x & Piece::Queen || square[i].x & Piece::Rook)
      {
        return (true);
      }
      else if (square[i].x & Piece::King && curKingPos - i <= 1)
      {
        return (true);
      }
      else
        break;
    }

    for (int i = curKingPos - 9; i >= 0 && i % 8 != 7; i -= 9)
    {
      if (square[i].x == 0 || i == from)
        continue;
      else if (square[i].x & colour || i == to)
        break;
      else if (square[i].x & Piece::Queen || square[i].x & Piece::Bishop)
      {
        return (true);
      }
      else if (square[i].x & Piece::King && curKingPos - i <= 9)
      {
        return (true);
      }
      else if (square[i].x & Piece::Pawn && curKingPos - i <= 9 && colour & Piece::White)
      {
        return (true);
      }
      else
        break;
    }

    return (false);
  }

  // Evaluates the board and returns a score
  int evaluate()
  {
    int score = 0;
    for (int i = 0; i < 64; i++)
    {
      if (square[i].x == Piece::None)
        continue;
      switch (square[i].x)
      {
      case Piece::Pawn | Piece::White:
        score += 1;
        break;
      case Piece::Pawn | Piece::Black:
        score -= 1;
        break;
      case Piece::Bishop | Piece::White:
        score += 3;
        break;
      case Piece::Bishop | Piece::Black:
        score -= 3;
        break;
      case Piece::Knight | Piece::White:
        score += 3;
        break;
      case Piece::Knight | Piece::Black:
        score -= 3;
        break;
      case Piece::Rook | Piece::White:
        score += 5;
        break;
      case Piece::Rook | Piece::Black:
        score -= 5;
        break;
      case Piece::Queen | Piece::White:
        score += 9;
        break;
      case Piece::Queen | Piece::Black:
        score -= 9;
        break;
      case Piece::King | Piece::White:
        score += 1000;
        break;
      case Piece::King | Piece::Black:
        score -= 1000;
        break;
      }
    }
    return score;
  }

  int kingFind(uint8_t colour)
  {
    int kingPos = -1;
    for (int i = 0; i < 63; i++)
    {
      if (square[i].x == colour + Piece::King)
      {
        kingPos = i;
        break;
      }
    }
    return (kingPos);
  }

  string bestMove(uint8_t colour)
  {
    int depth = DEPTH;
    int bestScore = colour == Piece::White ? -100000 : 100000;
    string bestMove = "";
    vector<string> moves = findPossibleMoves(colour);
    if (moves.size() == 1)
      return moves[0];
    for (int i = 0; i < moves.size(); i++)
    {
      Board board = *this;
      board.makeMove(moves[i]);
      int score = board.minimax(depth, colour == Piece::White ? Piece::Black : Piece::White);
      if (colour == Piece::White && score > bestScore)
      {
        bestScore = score;
        bestMove = moves[i];
      }
      else if (colour == Piece::Black && score < bestScore)
      {
        bestScore = score;
        bestMove = moves[i];
      }
    }
    return bestMove;
  }

  int minimax(int depth, uint8_t colour, int alpha = -100000, int beta = 100000)
  {
    if (depth == 0)
      return evaluate();
    vector<string> moves = findPossibleMoves(colour);
    if (colour == Piece::White)
    {
      int bestScore = -100000;
      for (int i = 0; i < moves.size(); i++)
      {
        Board board = *this;
        board.makeMove(moves[i]);
        int score = board.minimax(depth - 1, Piece::Black, alpha, beta);
        bestScore = max(bestScore, score);
        alpha = max(alpha, score);
        if (beta <= alpha)
        {
          break;
        }
      }
      return bestScore;
    }
    else
    {
      int bestScore = 100000;
      for (int i = 0; i < moves.size(); i++)
      {
        Board board = *this;
        board.makeMove(moves[i]);
        int score = board.minimax(depth - 1, Piece::White, alpha, beta);
        bestScore = min(bestScore, score);
        beta = min(beta, score);
        if (beta <= alpha)
        {
          break;
        }
      }
      return bestScore;
    }
    /*
    if (depth == 0)
      return evaluate();
    int bestScore = colour == Piece::White ? -100000 : 100000;
    vector<string> moves = findPossibleMoves(colour);
    for (int i = 0; i < moves.size(); i++)
    {
      Board board = *this;
      board.makeMove(moves[i]);
      int score = board.minimax(depth - 1, colour == Piece::White ? Piece::Black : Piece::White);
      if (colour == Piece::White && score > bestScore)
        bestScore = score;
      else if (colour == Piece::Black && score < bestScore)
        bestScore = score;
    }
    return bestScore;
    */
  }

  // Print the board
  void print()
  {
    for (int i = 0; i < 64; i++)
    {
      string piece;
      switch (square[i].x)
      {
      case Piece::None:
        piece = "--";
        break;
      case Piece::Pawn | Piece::White:
        piece = "WP";
        break;
      case Piece::Pawn | Piece::Black:
        piece = "BP";
        break;
      case Piece::Bishop | Piece::White:
        piece = "WB";
        break;
      case Piece::Bishop | Piece::Black:
        piece = "BB";
        break;
      case Piece::Knight | Piece::White:
        piece = "WN";
        break;
      case Piece::Knight | Piece::Black:
        piece = "BN";
        break;
      case Piece::Rook | Piece::White:
        piece = "WR";
        break;
      case Piece::Rook | Piece::Black:
        piece = "BR";
        break;
      case Piece::Queen | Piece::White:
        piece = "WQ";
        break;
      case Piece::Queen | Piece::Black:
        piece = "BQ";
        break;
      case Piece::King | Piece::White:
        piece = "WK";
        break;
      case Piece::King | Piece::Black:
        piece = "BK";
        break;
      }
      cout << piece << " ";
      if ((i + 1) % 8 == 0)
        cout << endl;
    }
    cout << "Evaluation: " << evaluate() << endl;
  }
};

void aiMove(Board &board, uint8_t ai)
{
  vector<string> moves = board.findPossibleMoves(ai);
  int kingPos = board.kingFind(ai);
  if (moves.size() == 0)
  {
    if (board.inCheck(ai, board.toAlgebraic(kingPos) + board.toAlgebraic(kingPos), kingPos))
      cout << (ai == Piece::White ? "White" : "Black") << " got checkmated." << endl;
    else
      cout << "Stalemate." << endl;
    return;
  }
  for (int i = 0; i < moves.size(); i++)
    cout << moves[i] << ",";
  string bestMove = board.bestMove(ai);
  cout << endl
       << "Making move: " << bestMove << endl;
  board.makeMove(bestMove);
  board.print();
}
void playerMove(Board &board)
{
  string move;
  cout << "Make a move..." << endl;
  cin >> move;
  board.makeMove(move);
  board.print();
}
void moveIterator(int argc, char *argv[])
{
  Board board;
  board.print();
  string move;
  if (argc == 1) // player vs player for testing
  {
    while (true)
      playerMove(board);
    return;
  }

  string aiColorStr = argv[1];
  if (aiColorStr == "both")
  {
    while (true)
    {
      system("pause");
      aiMove(board, Piece::White);
      system("pause");
      aiMove(board, Piece::Black);
    }
    return;
  }
  uint8_t ai = aiColorStr == "white" ? Piece::White : Piece::Black;

  if (ai == Piece::White) // ai is white, player is black
  {
    aiMove(board, ai);
  }
  while (true)
  {
    playerMove(board);
    aiMove(board, ai);
  }
}
int main(int argc, char *argv[])
{
  moveIterator(argc, argv);
  return 0;
}