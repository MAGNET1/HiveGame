#pragma once
#include "headers.h"
#include "Hive.h"

class Hive;

class TranspositionTable {
	std::unordered_multimap<std::string, entry> map;

public:
	void insert(Hive& h, int heuristics, int depth, int state, playerMove_t move);
	entryWithInfo probe(Hive& h, int depth, int alpha, int beta);
	void clear();
};