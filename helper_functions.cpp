#include "helper_functions.h"
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>

using namespace std;

int selectRandom(vector<string> moves)
{
    return std::rand() % moves.size();
}

int selectBestTake(vector<string> moves)
{
    for (string move :moves)
    {
        int from = getFrom(move);
        int to = getTo(move);
    }
    return -1;
}

int getFrom(string move)
{
    return (move[0] - 'a') + 56 - (move[1] - '1') * 8;
}

int getTo(string move)
{
    return (move[2] - 'a') + 56 - (move[3] - '1') * 8;
}
