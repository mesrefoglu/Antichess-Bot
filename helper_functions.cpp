#include "helper_functions.h"
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>

using namespace std;

#include "piece.h"

int selectRandom(vector<string> moves)
{
    return std::rand() % moves.size();
}

string selectBestTake(vector<string> moves, const Piece *square)
{
    string best_take = moves[0];
    if (square[1].x > Piece::None)
    {
        int max = Piece::None;
        int min = Piece::King;
        for (string move : moves)
        {
            int to = getTo(move);
            int from = getFrom(move);
            if (square[to].x > max)
            {
                max = square[to].x;
                best_take = move;
                min = square[from].x;
            }
            else if (square[to].x == max && max > Piece::None)
            {
                if (min > square[from].x)
                {
                    min = square[from].x;
                    best_take = move;
                }
            }
        }
        cout << "\nMaking move: " << best_take << endl;
        return best_take;
    }
    else
    {
        return moves[selectRandom(moves)];
    }
}

int getFrom(string move)
{
    return (move[0] - 'a') + 56 - (move[1] - '1') * 8;
}

int getTo(string move)
{
    return (move[2] - 'a') + 56 - (move[3] - '1') * 8;
}
