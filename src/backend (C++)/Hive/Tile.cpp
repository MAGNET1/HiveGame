#include "tile.h"

Tile::Tile(std::string tileName, int tileID, int value) {
	name = tileName;
	ID = tileID;
	onTop = "";
	beetleOnTop = false;
	willBreakHive = false;
	heuristicValue = value;
	for (int i = 0; i < 6; i++) {
		neighbourhood[i] = "";
	}
}

Tile::Tile(const Tile& t)
{
	this->name = t.name;
	this->onTop = t.onTop;
	this->beetleOnTop = t.beetleOnTop;
	this->position = t.position;
	this->ID = t.ID;
	this->willBreakHive = t.willBreakHive;
	this->heuristicValue = t.heuristicValue;
	for (int i = 0; i < 6; i++) {
		this->neighbourhood[i] = t.neighbourhood[i];
	}
}

int Tile::getID() {
	return ID;
}

bool Tile::hasNeighbour(int direction) {
	return neighbourhood[direction] != "";
}

std::string Tile::getNeighbour(int direction) {
	return neighbourhood[direction];
}

intPair_t Tile::getCoords() {
	return position;
}

std::string Tile::getOnTop() {
	return onTop;
}

std::string Tile::getName() {
	return name;
}

void Tile::setOnTop(std::string which) {
	onTop = which;
}

bool Tile::isBeetleOnTop() {
	return beetleOnTop;
}

void Tile::setBeetleOnTop(bool val) {
	beetleOnTop = val;
}

void Tile::setCoords(intPair_t coords) {
	position = coords;
}

pairVector Tile::getNeighbourhoodCoords() {
	pairVector vec;
	vec.push_back(intPair_t(position.first, position.second + 2));
	vec.push_back(intPair_t(position.first + 1, position.second + 1));
	vec.push_back(intPair_t(position.first + 1, position.second - 1));
	vec.push_back(intPair_t(position.first, position.second - 2));
	vec.push_back(intPair_t(position.first - 1, position.second - 1));
	vec.push_back(intPair_t(position.first - 1, position.second + 1));
	return vec;
}

stringVector Tile::getNeighbourhood() {
	stringVector vec;
	for (int i = 0; i < 6; i++) {
		vec.push_back(getNeighbour(i));
	}
	return vec;
}

void Tile::setNeighbour(int direction, std::string who) {
	neighbourhood[direction] = who;
}

int Tile::neighbourByCoords(intPair_t coords) {
	if (position.first == coords.first && position.second == coords.second - 2)
		return NN;
	if (position.first == coords.first - 1 && position.second == coords.second - 1)
		return NE;
	if (position.first == coords.first - 1 && position.second == coords.second + 1)
		return SE;
	if (position.first == coords.first && position.second == coords.second + 2)
		return SS;
	if (position.first == coords.first + 1 && position.second == coords.second + 1)
		return SW;
	if (position.first == coords.first + 1 && position.second == coords.second - 1)
		return NW;

	return -1;
}

bool Tile::isSurrounded() {
	bool isTileSurrounded = true;
	for (int i = 0; i < 6; i++) {
		if (neighbourhood[i] == "")
			isTileSurrounded = false;
	}

	return isTileSurrounded;
}

int Tile::getNeighourCount() {
	int n = 0;
	for (int i = 0; i < 6; i++) {
		if (neighbourhood[i] != "")	++n;
	}
	return n;
}

bool Tile::willBreak() {
	return willBreakHive;
}

void Tile::willBreakUpdate(bool value) {
	willBreakHive = value;
}

int Tile::getHeuristicValue() {
	return heuristicValue;
}