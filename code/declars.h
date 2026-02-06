// declarations

#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <fstream>
#include <future>
#include <immintrin.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace std;


// #define DO_HCE

typedef long long ll;
typedef unsigned long long ull;
// typedef __int128 lll;
typedef long double ld;
const ll mod = 1e9 + 7, inf = 1e8 + 1;
const ll noneCode = inf / 173;
const ld eps = 1e-9, pi = 3.1415926;
#define all(a) a.begin(), a.end()
#define mpp make_pair
#define F first
#define S second

mt19937_64 rngT(chrono::steady_clock::now().time_since_epoch().count());
mt19937_64 rng(1898129380);

const int maxDepth = 256;

const int EMPTY = 2, WHITE = 0, BLACK = 1, ERROR = -2;
const int NOPIECE = 0, PAWN = 1, KNIGHT = 2, BISHOP = 3, ROOK = 4, QUEEN = 5, KING = 6;
const int DONT_SORT = 0, DO_SORT = 1;
const int ALL_MOVES = 0, ONLY_CAPTURES = 1;
const int NO_EN_PASSANT = 10;

const int LOWER_BOUND = 0, EXACT = 1, UPPER_BOUND = 2, NONE = 3;
const int NO_EVAL = 32767 - 300; // return if we can't use evaluation from the transposition table

const int NONE_SCORE = 32767 - 300; // score which never occures
const int MATE_SCORE = 32767, DRAW_SCORE = 0;
const int MATE_SCORE_MAX_PLY = MATE_SCORE - 257;

#define memoryUsageMB 64

float lmrLogTable[257][257];

void initLmrTable() {
	for (int i = 1; i <= 256; i++)
		for (int j = 1; j <= 256; j++)
			lmrLogTable[i][j] = log(float(i)) * log(float(j)) / 3.0;
}

vector<string> splitStr(string s, string c) { // splits a string by (c) substrings (for parsing)
    s += c;
    vector<string> an;
    string t;
    for (auto i : s) {
        t.push_back(i);
        if (t.length() >= c.length()) {
            if (t.substr(t.length() - c.length(), c.length()) == c) {
                t = t.substr(0, t.length() - c.length());
                if (t != "")
                    an.push_back(t);
                t = "";
            }
        }
    }
    return an;
}

string squareNumberToUCI(int square) {
    string s;
    s.push_back(char('a' + (square & 7)));
    s.push_back(char('0' + 8 - (square >> 3)));
    return s;
}

string intTo5symbFormat(int x) { // for eval diagram
    string s;
    if (x >= 0)
        s += "+";
    else
        s += "-";
    x = abs(x);
    s += to_string(x / 100);
    s.push_back('.');
    x %= 100;
    if (x < 10)
        s += "0";
    s += to_string(x);
    while (s.size() > 5)
        s.pop_back();
    return s;
}

int normalizeNNUEscore(int score, int material) {
    int x = material;
    int p_a = ((49.326 * x / 58 + -82.882) * x / 58 + -128.585) * x / 58 + 421.978;
    int p_b = ((61.170 * x / 58 + -266.040) * x / 58 + 420.050) * x / 58 + 24.085;
    // constexpr double as[] = {49.32556867, -82.88198999, -128.58538737, 421.97786682};
    // constexpr double bs[] = {61.17007333, -266.04041076, 420.05003873, 24.08542565};

    return (score * 100.0 / p_a);
}