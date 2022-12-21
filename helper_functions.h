#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>

using namespace std;

#include "piece.h"

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

int selectRandom(vector<string> moves);
string selectBestTake(vector<string> moves, const Piece *square);
int getFrom(string move);
int getTo(string move);

#endif
