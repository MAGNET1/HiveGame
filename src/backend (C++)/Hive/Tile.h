#pragma once
#pragma once

#include "headers.h"

class Tile {
	std::string name;
	int ID;
	std::string neighbourhood[6];
	intPair_t position;
	std::string onTop;
	bool beetleOnTop;
	bool willBreakHive;
	int heuristicValue;

public:
	Tile(std::string tileName, int tileID, int value);
	Tile(const Tile& t);
	bool hasNeighbour(int direction);
	pairVector getNeighbourhoodCoords();
	stringVector getNeighbourhood();
	std::string getNeighbour(int direction);
	intPair_t getCoords();
	std::string getOnTop();
	std::string getName();
	void setOnTop(std::string which);
	bool isBeetleOnTop();
	void setBeetleOnTop(bool val);
	void setCoords(intPair_t coords);
	void setNeighbour(int direction, std::string who);
	int neighbourByCoords(intPair_t coords);
	bool isSurrounded();
	int getID();
	int getNeighourCount();
	bool willBreak();
	void willBreakUpdate(bool value);
	int getHeuristicValue();
};