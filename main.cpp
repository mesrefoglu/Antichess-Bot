#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>
#include <tuple>
using namespace std;

#include "helper_functions.h"
#include "piece.h"
#include "board.h"
#include "treenode.h"

int main(int argc, char *argv[])
{
  std::srand(std::time(nullptr));
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
  uint8_t ai = (argv1 == "white" ? Piece::White : Piece::Black);
  if (argv1 == "white") // ai is white, player is black
  {
    vector<string> moves = board.findPossibleMoves(ai);
    /*for (int i = 0; i < moves.size(); i++)
      cout << moves[i] << ", ";
    cout << endl;*/
    int choice = std::rand() % moves.size();
    board.makeMove(moves[choice]);

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
    // vector<string> moves = board.findPossibleMoves(ai);
    // cout << "Possible moves:" << endl;
    // for (int i = 0; i < moves.size(); i++)
    //   cout << moves[i] << ", ";
    int depth = 2;
    tuple<string, int> suggestedmove;
    TreeNode currentPosition = TreeNode(Piece::White, board);
    suggestedmove = currentPosition.findBestMove(depth);
    board.makeMove(get<0>(suggestedmove));
    board.print();
    cout << endl;
  }

  return 0;
}
