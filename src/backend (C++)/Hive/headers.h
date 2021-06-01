#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <utility>
#include <regex>
#include <algorithm>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <chrono>
#include <winsock2.h>
#include <Windows.h>
#include <thread>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "SocketServer.h"
#include <omp.h>

#define NN 0
#define NE 1
#define SE 2
#define SS 3
#define SW 4
#define NW 5
#define ON 6
#define FIRST 7

#define WHITE 0
#define BLACK 1

#define STATE_EXACT 0
#define STATE_ALPHA 1
#define STATE_BETA 2

#define TRANSPOSITION_TABLE
#define MOVE_ORDERING
// #define THREADED

#define THREADS_NUM 12
#define MINIMAX_DEPTH 4

#define POINTS_PIN_BASE_MULTIPLIER 1
#define POINTS_QUEEN_VALUE 50
#define POINTS_ANT_VALUE 40
#define POINTS_GRASSHOPPER_VALUE 30
#define POINTS_BEETLE_VALUE 30
#define POINTS_SPIDER_VALUE 10

extern char digits[];
extern char bugs[];
extern char bugs3[];
extern char bugs2[];

extern int bugs3HeuristicValue[];
extern int bugs2HeuristicValue[];

extern char players[];

extern double tableTime;

extern std::unordered_map<std::string, int> TileIDs;

typedef std::pair<int, int> intPair_t;
typedef std::vector<std::pair<int, int>> pairVector;
typedef std::vector<std::string> stringVector;
typedef std::set<std::pair<int, int>> intPairSet;
typedef std::map<std::string, pairVector> intPairMap;
typedef std::set<std::pair<int, int>> pairSet;
typedef std::pair<std::string, std::pair<int, int>> playerMove_t;
typedef std::pair<playerMove_t, int> playerMoveWithEval_t;

struct moveCompose {
	std::string which;
	std::string where;
	int direction;
};

struct availableMoves {
	intPairMap tilesPlayed;
	pairSet tilesUnplayed;
};

struct HashKey {
	size_t operator()(std::string x) const {
		size_t hash = 5381;

		for (auto c : x)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}
};

struct entry {
	int heuristic;
	int depth;
	int state;
	playerMove_t bestMove;
};

struct entryWithInfo {
	entry e;
	bool success;
};

typedef std::unordered_multimap<std::string, entry>::iterator mapIterator_t;