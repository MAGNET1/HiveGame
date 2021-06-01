#pragma once
#include "headers.h"
#include "tile.h"
#include "TranspositionTable.h"

/*
	Bugs:
	Q - Queen
	S - Spider
	B - Beetle
	G - Grasshopper
	A - Ant
*/

class Hive {
	char winner;
	int move;
	int movePlayer[2];
	char whoMoves;
	int tilesPlayedCount;
	std::unordered_map<std::string, Tile*> tilesPlayed;
	std::unordered_map<std::string, Tile*> tilesUnplayed[2];
	playerMove_t recentMoves[2][4];
	bool hadNoMoves[2];
	playerMove_t minimaxMove;
	bool hiveBrakeTab[22];
	std::string hash;

public:

	static const char MOVE_WHITE = 'w';
	static const char MOVE_BLACK = 'b';
	Hive(char whoMovesFirst);
	Hive(const Hive& h);
	~Hive();

	friend class TranspositionTable;
	friend std::vector<playerMove_t> buildMovesVector(Hive& h, struct availableMoves moves);

	std::string buildName(char color, char bug, int number);
	intPair_t translateToCoords(Tile* tile, int direction);
	bool moveTile(char player, std::string which, std::string where, int direction);
	void placeTileSocket(char player, std::string which, intPair_t where);
	bool placeTile(char player, std::string which, std::string where, int direction);
	void moveTileSocket(char player, std::string which, intPair_t where);
	std::string buildMovesJSON(bool isOver);
	std::string buildMovesJSON_AI(bool isOver, playerMove_t AI_Move);
	bool IsValidTurn(char player, std::string which);
	bool IsValidMove(char player, std::string which, intPair_t coords);
	bool willBrakeHive(std::string which);
	void exploreHive(Tile* tile, bool(&brakeTab)[22]);
	void removeTile(std::string which);

	int getPlayerMove(int color);

	struct availableMoves getAllMoves(char player);
	pairVector getAvailableMoves(std::string which);
	pairVector getFlipMoves(std::string which);
	pairVector getQueenMoves(std::string which);
	pairVector getSpiderMoves(std::string which);
	pairVector getAntMoves(std::string which);
	pairVector getGrasshopperMoves(std::string which);
	pairVector getBeetleMoves(std::string which);
	char getTopColor(std::string which);
	std::string getTopTile(std::string which);
	char pieceColor(std::string str);
	char pieceKind(std::string str);
	bool isTileInGame(std::string str);
	bool isTileUnplayed(char player, std::string str);
	int translatePlayer(char p);
	bool isVoid(std::string str);
	int getCounterDirection(int direction);
	int getRounded(int x);
	void nextTurn(bool skipped=false);
	void attachTile(std::string which, intPair_t coords);
	void attachTileBeetle(std::string which, intPair_t coords);
	char getWinner();
	void playerMove();
	void playerMoveSocket(SocketServer &server, bool AI);
	pairSet getNeighbourFields(char player);
	struct moveCompose ParseUserInput(std::string input);
	bool isGameOver();
	void removeFromSet(intPairSet& vec, intPairSet toPrune);

	// for AI
	char getWhoMoves();
	std::map<std::string, Tile*> getTilesUnplayed(int player);
	void computerMove(SocketServer& server, playerMove_t move);
	int getHeuristicEvaluation(int depth, bool debug = false);
	int getAmountOfNeighbours(std::string which);
	playerMove_t getMinimaxMove();
	void MinimaxMove(playerMove_t move);
	void updateHash(std::string which, std::pair<int, int> where);
	std::string getHash();
	void updateWillBreakHive();

	std::string coordsToNeighbour(std::pair<int, int> coords);
};