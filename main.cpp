#include <iostream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

// A Piece is represented as 8-bits
// Leftmost 2-bits represent color
// Rightmost 6-bits represent type of piece
class Piece
{
public:
  uint8_t x;
  bool hasMoved = false;
  bool enPassantable = false;

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
    int from = (move[0] - 'a') + 56 - (move[1] - '1') * 8;
    int to = (move[2] - 'a') + 56 - (move[3] - '1') * 8;
    square[to].x = square[from].x;
    square[to].hasMoved = true;
    square[from] = Piece(Piece::None);
    // Reset en passantable flags
    for (int i = 0; i < 64; i++)
    {
      if (i != to) // Don't reset the flag for the square the piece is moving to (in case it's enPassantable)
        square[i].enPassantable = false;
    }
    // Promotion Case:
    if (move.length() == 5)
    {
      if (move[4] == 'q')
        square[to].x = Piece::Queen | (square[to].x & Piece::White);
      else if (move[4] == 'r')
        square[to].x = Piece::Rook | (square[to].x & Piece::White);
      else if (move[4] == 'b')
        square[to].x = Piece::Bishop | (square[to].x & Piece::White);
      else if (move[4] == 'n')
        square[to].x = Piece::Knight | (square[to].x & Piece::White);
    }
    // Castling Case:
    else if ((square[to].x & 63) == Piece::King)
    {
      cout << "Got here" << endl;
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
      if (to == 58)
      {
        square[56] = Piece(Piece::None);
        square[59] = Piece(Piece::Rook | Piece::White);
      }
      else if (to == 62)
      {
        cout << "Should be here" << endl;
        square[63] = Piece(Piece::None);
        square[61] = Piece(Piece::Rook | Piece::White);
      }
    }
    // En Passant Case:
    else if ((square[to].x & 63) == Piece::Pawn)
    {
      if (to == from + 16)
        square[from + 8].enPassantable = true;
      else if (to == from - 16)
        square[from - 8].enPassantable = true;
      else if ((to == from + 7 || to == from + 9) && square[to].enPassantable)
        square[to - 8] = Piece(Piece::None);
      else if ((to == from - 7 || to == from - 9) && square[to].enPassantable)
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
    bool takeFound;
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
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 9));
            break;
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 9));
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
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 7));
            break;
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 7));
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
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 9));
            break;
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 9));
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
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 7));
            break;
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 7));
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
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j));
            break;
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j));
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
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j));
            break;
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j));
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
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 8));
            break;
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + j * 8));
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
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 8));
            break;
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - j * 8));
        }
      }
      else if (piece == Piece::Pawn)
      {
        if (colour == Piece::White)
        {
          if (i % 8 != 7 && (square[i - 7].x & opposite || square[i - 7].enPassantable))
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7));
          }
          if (i % 8 != 0 && (square[i - 9].x & opposite || square[i - 9].enPassantable))
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 9));
          }
          if (!takeFound && square[i - 8].x == Piece::None)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
          }
          if (!takeFound && !square[i].hasMoved && square[i - 16].x == Piece::None)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 16));
          }
        }
        else
        {
          if (i % 8 != 0 && (square[i + 7].x & opposite || square[i + 7].enPassantable))
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 7));
          }
          if (i % 8 != 7 && (square[i + 9].x & opposite || square[i + 9].enPassantable))
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 9));
          }
          if (square[i + 8].x == Piece::None && !takeFound)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
          }
          if (square[i + 16].x == Piece::None && !square[i].hasMoved && !takeFound)
          {
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 16));
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
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 17));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 17));
        }
        if (i + 15 < 64 && i % 8 >= 1 && !(square[i + 15].x & colour))
        {
          if (square[i + 15].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 15));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 15));
        }
        if (i + 10 < 64 && i % 8 <= 5 && !(square[i + 10].x & colour))
        {
          if (square[i + 10].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 10));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 10));
        }
        if (i + 6 < 64 && i % 8 >= 2 && !(square[i + 6].x & colour))
        {
          if (square[i + 6].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 6));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 6));
        }
        if (i - 17 >= 0 && i % 8 >= 1 && !(square[i - 17].x & colour))
        {
          if (square[i - 17].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 17));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 17));
        }
        if (i - 15 >= 0 && i % 8 <= 6 && !(square[i - 15].x & colour))
        {
          if (square[i - 15].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 15));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 15));
        }
        if (i - 10 >= 0 && i % 8 >= 2 && !(square[i - 10].x & colour))
        {
          if (square[i - 10].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 10));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 10));
        }
        if (i - 6 >= 0 && i % 8 <= 5 && !(square[i - 6].x & colour))
        {
          if (square[i - 6].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 6));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 6));
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
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 1));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 1));
        }
        if (i - 1 >= 0 && i % 8 - 1 >= 0 && !(square[i - 1].x & colour))
        {
          if (square[i - 1].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 1));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 1));
        }
        if (i + 8 < 64 && !(square[i + 8].x & colour))
        {
          if (square[i + 8].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 8));
        }
        if (i - 8 >= 0 && !(square[i - 8].x & colour))
        {
          if (square[i - 8].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 8));
        }
        if (i + 9 < 64 && i % 8 + 1 < 8 && !(square[i + 9].x & colour))
        {
          if (square[i + 9].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 9));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 9));
        }
        if (i - 9 >= 0 && i % 8 - 1 >= 0 && !(square[i - 9].x & colour))
        {
          if (square[i - 9].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 9));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 9));
        }
        if (i + 7 < 64 && i % 8 - 1 >= 0 && !(square[i + 7].x & colour))
        {
          if (square[i + 7].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 7));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i + 7));
        }
        if (i - 7 >= 0 && i % 8 + 1 < 8 && !(square[i - 7].x & colour))
        {
          if (square[i - 7].x & opposite)
          {
            if (!takeFound)
            {
              takeFound = true;
              moves.clear();
            }
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7));
          }
          else if (!takeFound)
            moves.push_back(toAlgebraic(i) + toAlgebraic(i - 7));
        }
      }
    }
    return moves;
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
};

int main(int argc, char *argv[])
{
  Board board;
  board.print();
  string move;
  if (argc == 1) // player vs player for testing
  {
    while (move != "end")
    {
      cout << "Make a move..." << endl;
      cin >> move;
      if (move == "end")
        break;
      board.makeMove(move);
      board.print();
    }
    return 0;
  }

  string argv1 = argv[1];
  uint8_t ai = argv1 == "white" ? Piece::White : Piece::Black;
  if (argv1 == "white") // ai is white, player is black
  {
    vector<string> moves = board.findPossibleMoves(ai);
    for (int i = 0; i < moves.size(); i++)
      cout << moves[i] << endl;
    board.makeMove(moves[0]);
    board.print();
  }
  while (move != "end")
  {
    cout << "Make a move..." << endl;
    cin >> move;
    if (move == "end")
      break;
    board.makeMove(move);
    board.print();
    vector<string> moves = board.findPossibleMoves(ai);
    cout << "Possible moves:" << endl;
    for (int i = 0; i < moves.size(); i++)
      cout << moves[i] << ", ";
    cout << "\nMaking move: " << moves[0] << endl;
    board.makeMove(moves[0]);
    board.print();
  }

  return 0;
}