#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>

using namespace std;

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

int selectRandom(vector<string> moves);
int selectBestTake(vector<string> moves);
int getFrom(string move);
int getTo(string move);

#endif
