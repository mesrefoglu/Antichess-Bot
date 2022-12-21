#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>

#include "piece.h"
#include "board.h"

#ifndef TREENODE_H
#define TREENODE_H

class TreeNode
{
public:
  Board tempBoard;
  int color_to_move;
  vector<string> possibleMoves;

  // Initialize the node
  TreeNode(int to_move, Board current_board);
  ~TreeNode();

  tuple<string, int> findBestMove(int depth);
};

#endif
