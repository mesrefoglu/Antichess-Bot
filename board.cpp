#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <ctime>

using namespace std;

#include "piece.h"
#include "board.h"
#include "helper_functions.h"

Board::Board()

public:
  Piece square[64];
  int enPassantable = -1;
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
  // Coordinate algebraic notation, examples:
  // Normal move: e2e4
  // Pawn promotion: e7e8q
  // Castling as the King's two-square move: e1g1
  // En Passant, requires storage of last move: e5f6
  void makeMove(string move)
  {
    int from = getFrom(move);
    int to = getTo(move);
    square[to].x = square[from].x;
    square[to].hasMoved = true;
    square[from] = Piece(Piece::None);
    // Reset en passantable flags
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
    else if ((square[to].x & 63) == Piece::King)
    {
      if (to == 2)
      {
        square[0] = Piece(Piece::None);
        square[3] = Piece(Piece::Rook | Piece::Black);
      }
      else if (to == 6)
      {
        square[7] = Piece(Piece::None);
        square[5] = Piece(Piece::Rook | Piece::Black);
      }
      else if (to == 58)
      {
        square[56] = Piece(Piece::None);
        square[59] = Piece(Piece::Rook | Piece::White);
      }
      else if (to == 62)
      {
        square[63] = Piece(Piece::None);
        square[61] = Piece(Piece::Rook | Piece::White);
      }
    }
    // En Passant Case:
    else if ((square[to].x & 63) == Piece::Pawn)
    {
      if (to == from + 16)
        enPassantable = from + 8;
      else if (to == from - 16)
        enPassantable = from - 8;
      else if ((to == from + 7 || to == from + 9) && enPassantable == to)
        square[to - 8] = Piece(Piece::None);
      else if ((to == from - 7 || to == from - 9) && enPassantable == to)
        square[to + 8] = Piece(Piece::None);
    }
  }

  string toAlgebraic(int square)
  {
    string result;
    result += (char)('a' + square % 8);
    result += (char)('1' + 7 - square / 8);
    return result;
  }

  vector<string> findPossibleMoves(uint8_t colour)
  {
	  vector<string> moves;
	  uint8_t opposite = colour == Piece::White ? Piece::Black : Piece::White;
	  bool takeFound = false;
	  int kingPos = kingFind(colour); // Used to store own King's Position

	  // Check whether in check at start of turn
	  bool inCheck = checkCheck(colour, toAlgebraic(kingPos) + toAlgebraic(kingPos), kingPos);
	  

	  // In Check, we may need to make non-take moves
	  // However, take moves should still have priority if possible
	  // Create new vector to store non-take moves whilst in check
	  vector<string> secondary;

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
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 9));
					  break;
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 9));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + j * 9));
			  }
			  for (int j = 1; i + j * 7 < 64 && i % 8 - j >= 0; j++)
			  {
				  if (square[i + j * 7].x & colour)
					  break;
				  if (square[i + j * 7].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 7));
					  break;
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 7));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + j * 7));
			  }
			  for (int j = 1; i - j * 9 >= 0 && i % 8 - j >= 0; j++)
			  {
				  if (square[i - j * 9].x & colour)
					  break;
				  if (square[i - j * 9].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 9));
					  break;
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 9));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - j * 9));
			  }
			  for (int j = 1; i - j * 7 >= 0 && i % 8 + j < 8; j++)
			  {
				  if (square[i - j * 7].x & colour)
					  break;
				  if (square[i - j * 7].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 7));
					  break;
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 7));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - j * 7));
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
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + j));
					  break;
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + j));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + j));
			  }
			  for (int j = 1; i - j >= 0 && i % 8 - j >= 0; j++)
			  {
				  if (square[i - j].x & colour)
					  break;
				  if (square[i - j].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - j));
					  break;
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - j));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - j));
			  }
			  for (int j = 1; i + j * 8 < 64; j++)
			  {
				  if (square[i + j * 8].x & colour)
					  break;
				  if (square[i + j * 8].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 8));
					  break;
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 8));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + j * 8));
			  }
			  for (int j = 1; i - j * 8 >= 0; j++)
			  {
				  if (square[i - j * 8].x & colour)
					  break;
				  if (square[i - j * 8].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 8));
					  break;
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 8));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - j * 8));
			  }
		  }
		  else if (piece == Piece::Pawn)
		  {
			  if (colour == Piece::White)
			  {
				  if (i % 8 != 7 && (square[i - 7].x & opposite || enPassantable == i - 7))
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  // Check for promotion
					  if (i - 7 < 8)
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7) + 'q');
					  else
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7));
				  }
				  if (i % 8 != 0 && (square[i - 9].x & opposite || enPassantable == i - 9))
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  // Check for promotion
					  if (i - 9 < 8)
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 9) + 'q');
					  else
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 9));
				  }
				  if (!takeFound && square[i - 8].x == Piece::None && !inCheck)
				  {
					  // Check for promotion
					  if (i - 8 < 8)
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 8) + 'q');
					  else
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
				  }
				  if (square[i - 8].x == Piece::None && inCheck)
				  {
					  // Check for promotion
					  if (i - 8 < 8)
						  secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 8) + 'q');
					  else
						  secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
				  }
				  if (!takeFound && !square[i].hasMoved && square[i - 8].x == Piece::None && square[i - 16].x == Piece::None)
				  {
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 16));
				  }
				  if (!square[i].hasMoved && square[i - 8].x == Piece::None && square[i - 16].x == Piece::None && inCheck)
				  {
					  secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 16));
				  }
			  }
			  else
			  {
				  if (i % 8 != 0 && (square[i + 7].x & opposite || enPassantable == i + 7))
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  // Check for promotion
					  if (i + 7 >= 56)
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 7) + 'q');
					  else
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 7));
				  }
				  if (i % 8 != 7 && (square[i + 9].x & opposite || enPassantable == i + 9))
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  // Check for promotion
					  if (i + 9 >= 56)
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 9) + 'q');
					  else
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 9));
				  }
				  if (!takeFound && square[i + 8].x == Piece::None && !inCheck)
				  {
					  // Check for promotion
					  if (i + 8 >= 56)
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 8) + 'q');
					  else
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
				  }
				  if (square[i + 8].x == Piece::None && inCheck)
				  {
					  // Check for promotion
					  if (i + 8 >= 56)
						  secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 8) + 'q');
					  else
						  secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
				  }
				  if (!takeFound && !square[i].hasMoved && square[i + 8].x == Piece::None && square[i + 16].x == Piece::None && !inCheck)
				  {
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 16));
				  }
				  if (!square[i].hasMoved && square[i + 8].x == Piece::None && square[i + 16].x == Piece::None && inCheck)
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
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 17));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + 17));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 17));
			  }
			  if (i + 15 < 64 && i % 8 >= 1 && !(square[i + 15].x & colour))
			  {
				  if (square[i + 15].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 15));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + 15));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 15));
			  }
			  if (i + 10 < 64 && i % 8 <= 5 && !(square[i + 10].x & colour))
			  {
				  if (square[i + 10].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 10));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + 10));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 10));
			  }
			  if (i + 6 < 64 && i % 8 >= 2 && !(square[i + 6].x & colour))
			  {
				  if (square[i + 6].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 6));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + 6));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 6));
			  }
			  if (i - 17 >= 0 && i % 8 >= 1 && !(square[i - 17].x & colour))
			  {
				  if (square[i - 17].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 17));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - 17));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 17));
			  }
			  if (i - 15 >= 0 && i % 8 <= 6 && !(square[i - 15].x & colour))
			  {
				  if (square[i - 15].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 15));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - 15));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 15));
			  }
			  if (i - 10 >= 0 && i % 8 >= 2 && !(square[i - 10].x & colour))
			  {
				  if (square[i - 10].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 10));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - 10));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 10));
			  }
			  if (i - 6 >= 0 && i % 8 <= 5 && !(square[i - 6].x & colour))
			  {
				  if (square[i - 6].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 6));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - 6));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 6));
			  }
		  }
		  else if (piece == Piece::King)
		  {
			  if (i + 1 < 64 && i % 8 + 1 < 8 && !(square[i + 1].x & colour))
			  {
				  if (square[i + 1].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 1));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + 1));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 1));
			  }

			  if (i - 1 >= 0 && i % 8 - 1 >= 0 && !(square[i - 1].x & colour))
			  {
				  if (square[i - 1].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 1));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - 1));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 1));
			  }
			  if (i + 8 < 64 && !(square[i + 8].x & colour))
			  {
				  if (square[i + 8].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
			  }
			  if (i - 8 >= 0 && !(square[i - 8].x & colour))
			  {
				  if (square[i - 8].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
			  }
			  if (i + 9 < 64 && i % 8 + 1 < 8 && !(square[i + 9].x & colour))
			  {
				  if (square[i + 9].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 9));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + 9));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 9));
			  }
			  if (i - 9 >= 0 && i % 8 - 1 >= 0 && !(square[i - 9].x & colour))
			  {
				  if (square[i - 9].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 9));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - 9));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i - 9));
			  }
			  if (i + 7 < 64 && i % 8 - 1 >= 0 && !(square[i + 7].x & colour))
			  {
				  if (square[i + 7].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i + 7));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i + 7));
				  else if (inCheck) secondary.push_back(toAlgebraic(i) + toAlgebraic(i + 7));
			  }
			  if (i - 7 >= 0 && i % 8 + 1 < 8 && !(square[i - 7].x & colour))
			  {
				  if (square[i - 7].x & opposite)
				  {
					  if (!takeFound)
					  {
						  takeFound = true;
						  if (!inCheck) moves.clear();
					  }
					  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7));
				  }
				  else if (!takeFound && !inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7));
				  else if (inCheck) moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7));
			  }
			  // Castling Case
			  // No take available, King has not moved, King not in check
			  if (!takeFound && square[i].hasMoved == false && !inCheck)
			  {
				  // Queenside Castle
				  if (square[i - 4].hasMoved == false && square[i - 1].x == Piece::None && square[i - 2].x == Piece::None && square[i - 3].x == Piece::None)
				  {
					  // Check if any of path to King's final spot threatened
					  // Final spot itself checked in final checkCheck
					  if (checkCheck(colour, toAlgebraic(i) + toAlgebraic(i - 1), kingPos))
					  {
						  moves.push_back(toAlgebraic(i) + toAlgebraic(i - 2));
					  }

				  }
				  // Kingside Castle
				  if (square[i + 3].hasMoved == false && square[i + 1].x == Piece::None && square[i + 2].x == Piece::None)
				  {
					  // Check if any of path to King's final spot threatened
					  // Final spot itself checked in final checkCheck
					  if (checkCheck(colour, toAlgebraic(i) + toAlgebraic(i + 1), kingPos)) moves.push_back(toAlgebraic(i) + toAlgebraic(i + 2));
				  }
			  }
		  }
	  }

	  std::cout << "Moves before check check" << std::endl;
	  for (auto& m : moves)
		  std::cout << m + ',';
	  std::cout << std::endl;
	  std::cout << "Secondary Moves:" << endl;
	  for (auto& m : secondary)
		  std::cout << m + ',';
	  std::cout << std::endl;
	  std::cout << "=======" << std::endl;

	  // Check Move validation
	  vector<string> checkedMoves;
	  for (auto move : moves) {
		  if (checkCheck(colour, move, kingPos) == false) checkedMoves.emplace_back(move);
	  }

	  // Check if we need to go to secondary moves
	  if (inCheck && checkedMoves.size() == 0) {
		  for (auto move : secondary) {
			  if (checkCheck(colour, move, kingPos) == false) checkedMoves.emplace_back(move);
		  }
	  }

	  return(checkedMoves);
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
  }

private:
  // "checkCheck" Design Philosophy:
  //    > should be called only by findPossibleMoves
  //    > Pass King's pos, to avoid recalculation
  //    > simulate the move
  //    > Only check necessary squares, in clockwise manner + Knight check
  // For each move, simulate board state, check if King in check
  bool checkCheck(uint8_t colour, string move, int kingPos)
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
              return(true);
          }
          else if (square[i].x & Piece::King && curKingPos - i <= 8)
          {
              return(true);
          }
          else break;
	  }
	  // Check for Knights
	  if ((curKingPos - 17 >= 0) && (curKingPos - 17 % 8 != 7) && square[curKingPos - 17].x == (opposite | Piece::Knight))
	  {
		  return(true);
	  }
	  else if ((curKingPos - 15 >= 0) && (curKingPos - 15 % 8 != 0) && square[curKingPos - 15].x == (opposite | Piece::Knight))
	  {
		  return(true);
	  }
	  else if ((curKingPos - 10 >= 0) && (curKingPos - 10 % 8 != 6) && (curKingPos - 10 % 8 != 7) && square[curKingPos - 10].x == (opposite | Piece::Knight))
	  {
		  return(true);
	  }
	  else if ((curKingPos - 6 >= 0) && (curKingPos - 6 % 8 != 0) && (curKingPos - 6 % 8 != 1) && square[curKingPos - 6].x == (opposite | Piece::Knight))
	  {
		  return(true);
	  }
	  else if ((curKingPos + 6 <= 63) && (curKingPos + 6 % 8 != 6) && (curKingPos - 10 % 8 != 7) && square[curKingPos + 6].x == (opposite | Piece::Knight))
	  {
		  return(true);
	  }
	  else if ((curKingPos + 10 <= 63) && (curKingPos + 10 % 8 != 0) && (curKingPos + 10 % 8 != 1) && square[curKingPos + 10].x == (opposite | Piece::Knight))
	  {
		  return(true);
	  }
	  else if ((curKingPos + 15 <= 63) && (curKingPos + 15 % 8 != 7) && square[curKingPos + 15].x == (opposite | Piece::Knight))
	  {
		  return(true);
	  }
	  else if ((curKingPos + 17 <= 63) && (curKingPos + 17 % 8 != 0) && square[curKingPos + 17].x == (opposite | Piece::Knight))
	  {
		  return(true);
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
			  return(true);
		  }
		  else if (square[i].x & Piece::King && curKingPos - i <= 7)
		  {
			  return(true);
		  }
		  else if (square[i].x & Piece::Pawn && curKingPos - i <= 8 && colour & Piece::White)
		  {
			  return(true);
		  }
		  else break;
	  }
	  for (int i = curKingPos + 1; i % 8 != 0; i++)
	  {
		  if (square[i].x == 0 || i == from)
			  continue;
		  else if (square[i].x & colour || i == to)
			  break;
		  else if (square[i].x & Piece::Queen || square[i].x & Piece::Rook)
		  {
			  return(true);
		  }
		  else if (square[i].x & Piece::King && i - curKingPos <= 1)
		  {
			  return(true);
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
			  return(true);
		  }
		  else if (square[i].x & Piece::King && i - curKingPos <= 9)
		  {
			  return(true);
		  }
		  else if (square[i].x & Piece::Pawn && i - curKingPos <= 9 && colour & Piece::Black)
		  {
			  return(true);
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
			  return(true);
		  }
		  else if (square[i].x & Piece::King && i - curKingPos <= 8)
		  {
			  return(true);
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
			  return(true);
		  }
		  else if (square[i].x & Piece::King && i - curKingPos <= 7)
		  {
			  return(true);
		  }
		  else if (square[i].x & Piece::Pawn && i - curKingPos <= 8 && colour & Piece::Black)
		  {
			  return(true);
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
			  return(true);
		  }
		  else if (square[i].x & Piece::King && curKingPos - i <= 1)
		  {
			  return(true);
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
			  return(true);
		  }
		  else if (square[i].x & Piece::King && curKingPos - i <= 9)
		  {
			  return(true);
		  }
		  else if (square[i].x & Piece::Pawn && curKingPos - i <= 9 && colour & Piece::White)
		  {
			  return(true);
		  }
		  else
			  break;
	  }

      return(false);
  }

  int kingFind(uint8_t colour) {
      int kingPos = -1;
      for (int i = 0; i < 63; i++) {
          if (square[i].x == colour + Piece::King) {
              kingPos = i;
              break;
          }
      }
      return(kingPos);
  }
}
