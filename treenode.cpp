#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <ctime>

using namespace std;

#include "piece.h"
#include "board.h"
#include "helper_functions.h"
#include "treenode.h"

TreeNode::TreeNode(int to_move, Board current_board){
	tempBoard.createBoard(current_board);
    color_to_move = to_move;
	possibleMoves = current_board.findPossibleMoves(color_to_move);
}

TreeNode::~TreeNode()
{
    possibleMoves.clear();
}

tuple<string, int> TreeNode::findBestMove(int depth) {
	if (depth == 0)
    {
        string best_move;
        int score;

        best_move = selectBestTake(possibleMoves, tempBoard.square);
        tempBoard.makeMove(best_move);
        score = tempBoard.evaluate();
		// use whatever evaluation you want to determine the best move
		//remember that you always want to find the best move of whichever color
		// is currently to play
        cout << "Eval move  " << best_move << endl;
        return make_tuple(best_move, score);
    }

    int next_to_move;

	if (color_to_move & Piece::Black)
    {
        next_to_move = Piece::White;
    }
	else
    {
		next_to_move = Piece::Black;
    }

	string best_move = "";
	int best_score;
    
    if (color_to_move & Piece::White)
    {
        best_score = -10000;
    }
    else
    {
        best_score = 10000;
    }

	for (int i = 0; i < int(possibleMoves.size()); i++)
    {
        string move = possibleMoves[i];
        cout << "checking move from possible " << move << " depth of " << depth << endl;
        Board newBoard;
        newBoard.createBoard(tempBoard);
        newBoard.makeMove(move);
        TreeNode cur_child = TreeNode(next_to_move, newBoard);
		tuple <string, int> values;
		values = cur_child.findBestMove(depth-1);
		
		/* Keep in mind that this comparison depends on whether you're evaluating
		the best move for white or black in this instance of the function. Regardless of which color your bot is, you
		always want to find the best move for whichever color is playing, otherwise
		your bot will only choose moves that win as quickly as possible if the opponent
		plays as badly as possible.
		*/ 
		if (get<1>(values) > best_score && color_to_move & Piece::White)
        {
			best_move = move;
			best_score = get<1>(values);
        }
        else if (get<1>(values) < best_score && color_to_move & Piece::Black)
        {
			best_move = move;
			best_score = get<1>(values);
        }
    }
    return make_tuple(best_move, best_score);
}
