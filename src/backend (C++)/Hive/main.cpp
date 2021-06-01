#include "headers.h"
#include "SocketServer.h"
#include "hive.h"
#include "tile.h"
#include "Game.h"

void fillTileIds() {
	TileIDs["wQ1"] = 0;
	TileIDs["wA1"] = 1;
	TileIDs["wA2"] = 2;
	TileIDs["wA3"] = 3;
	TileIDs["wG1"] = 4;
	TileIDs["wG2"] = 5;
	TileIDs["wG3"] = 6;
	TileIDs["wS1"] = 7;
	TileIDs["wS2"] = 8;
	TileIDs["wB1"] = 9;
	TileIDs["wB2"] = 10;

	TileIDs["bQ1"] = 11;
	TileIDs["bA1"] = 12;
	TileIDs["bA2"] = 13;
	TileIDs["bA3"] = 14;
	TileIDs["bG1"] = 15;
	TileIDs["bG2"] = 16;
	TileIDs["bG3"] = 17;
	TileIDs["bS1"] = 18;
	TileIDs["bS2"] = 19;
	TileIDs["bB1"] = 20;
	TileIDs["bB2"] = 21;
}


int main() {

	fillTileIds();
	SocketServer server;
	InitEngine(server);
}

// zmierzyc performance threaded vs non-threaded w mid-late game