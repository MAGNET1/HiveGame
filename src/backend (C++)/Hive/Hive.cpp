#include "hive.h"

Hive::Hive(char whoMovesFirst) {
	winner = '-';
	move = 0;
	movePlayer[0] = 0;
	movePlayer[1] = 0;
	hadNoMoves[0] = false;
	hadNoMoves[1] = false;
	hash += std::string(132, 'x');
	std::cout << hash;
	tilesPlayedCount = 0;
	whoMoves = whoMovesFirst;
	std::fill_n(hiveBrakeTab, 22, false);

	std::string tileIdentifier;
	for (int i = 0; i < 2; i++) {
		tileIdentifier = buildName(players[i], 'Q', 1);
		tilesUnplayed[i][tileIdentifier] = new Tile(tileIdentifier, TileIDs[tileIdentifier], POINTS_QUEEN_VALUE);

		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				tileIdentifier = buildName(players[i], bugs3[j], k + 1);
				tilesUnplayed[i][tileIdentifier] = new Tile(tileIdentifier, TileIDs[tileIdentifier], bugs2HeuristicValue[j]);

			}
		}
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				tileIdentifier = buildName(players[i], bugs2[j], k + 1);
				tilesUnplayed[i][tileIdentifier] = new Tile(tileIdentifier, TileIDs[tileIdentifier], bugs3HeuristicValue[j]);

			}
		}
	}

	playerMove_t blankMove;
	blankMove.first = "";
	blankMove.second = std::pair<int,int>(0,0);

	for (int i = 0; i < 4; i++) {
		recentMoves[0][i] = blankMove;
		recentMoves[1][i] = blankMove;
	}
}

Hive::Hive(const Hive& h) {
	winner = h.winner;
	move = h.move;
	movePlayer[0] = h.movePlayer[0];
	movePlayer[1] = h.movePlayer[1];
	whoMoves = h.whoMoves;
	hash = h.hash;

	tilesPlayedCount = h.tilesPlayedCount;
	std::copy(std::begin(h.hiveBrakeTab), std::end(h.hiveBrakeTab), std::begin(hiveBrakeTab));

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			recentMoves[i][j] = h.recentMoves[i][j];
		}
	}
	hadNoMoves[0] = h.hadNoMoves[0];
	hadNoMoves[1] = h.hadNoMoves[1];
	minimaxMove = h.minimaxMove;

	for (auto const& x : h.tilesUnplayed[WHITE]) {
		tilesUnplayed[WHITE][x.first] = new Tile(*x.second);
	}
	for (auto const& x : h.tilesUnplayed[BLACK]) {
		tilesUnplayed[BLACK][x.first] = new Tile(*x.second);
	}
	for (auto const& x : h.tilesPlayed) {
		tilesPlayed[x.first] = new Tile(*x.second);
	}


}

std::string Hive::getHash() {
	return hash;
}

void Hive::updateHash(std::string which, std::pair<int, int> where) {
	int offset = TileIDs[which] * 6;
	if (where.first < 0) {
		hash[offset] = '-';
	}
	else {
		hash[offset] = '+';
	}
	where.first = abs(where.first);
	if (where.first < 10) {
		hash[offset+1] = '0';
		hash[offset + 2] = where.first + '0';
	}
	else {
		hash.replace(offset+1, 2, std::to_string(where.first));
	}

	if (where.second < 0) {
		hash[offset+3] = '-';
	}
	else {
		hash[offset+3] = '+';
	}
	where.second = abs(where.second);
	if (where.second < 10) {
		hash[offset + 4] = '0';
		hash[offset + 5] = where.second + '0';
	}
	else {
		hash.replace(offset + 4, 2, std::to_string(where.second));
	}
}

std::string Hive::coordsToNeighbour(std::pair<int, int> coords) {
	int side = -1;
	for (auto x : this->tilesPlayed) {
		side = x.second->neighbourByCoords(coords);
		if (side == -1)
			continue;

		std::string str = x.first;

		if (side == 0) {
			str += "(N)";
		}
		else if (side == 1) {
			str += "(NE)";
		}
		else if (side == 2) {
			str += "(SE)";
		}
		else if (side == 3) {
			str += "(S)";
		}
		else if (side == 4) {
			str += "(SW)";
		}
		else if (side == 5) {
			str += "(NW)";
		}

		return str;
	}
	return std::string("-");
}

void Hive::MinimaxMove(playerMove_t move) {

	minimaxMove = move;

	recentMoves[translatePlayer(whoMoves)][3] = recentMoves[translatePlayer(whoMoves)][2];
	recentMoves[translatePlayer(whoMoves)][2] = recentMoves[translatePlayer(whoMoves)][1];
	recentMoves[translatePlayer(whoMoves)][1] = recentMoves[translatePlayer(whoMoves)][0];
	recentMoves[translatePlayer(whoMoves)][0] = move;

	if (!isTileInGame(move.first)) {
		placeTileSocket(whoMoves, move.first, move.second);
	}
	else {
		moveTileSocket(whoMoves, move.first, move.second);
	}

	updateHash(move.first, move.second);

	bool isOver = isGameOver();
	if (!isOver)
		nextTurn();
}

Hive::~Hive() {
	for (auto const& x : tilesUnplayed[WHITE]) {
		delete x.second;
	}
	for (auto const& x : tilesUnplayed[BLACK]) {
		delete x.second;
	}
	for (auto const& x : tilesPlayed) {
		delete x.second;
	}
}

playerMove_t Hive::getMinimaxMove() {
	return minimaxMove;
}


#define POINTS_QUEEN_SURROUNDING 1000
#define POINTS_QUEEN_SURROUNDING_OWN 700
#define POINTS_PIECES_ON_THE_BOARD 50

int Hive::getHeuristicEvaluation(int depth, bool debug) {
	int blackNeighbours = 0;
	int whiteNeighbours = 0;

	if (tilesPlayed.find("bQ1") != tilesPlayed.end())
		blackNeighbours = tilesPlayed[getTopTile("bQ1")]->getNeighourCount();
	if (tilesPlayed.find("wQ1") != tilesPlayed.end())
		whiteNeighbours = tilesPlayed[getTopTile("wQ1")]->getNeighourCount();


	if (whiteNeighbours == 6 && blackNeighbours == 6)	return 0;

	if (whiteNeighbours == 6)	return INT_MIN + 1 + (MINIMAX_DEPTH-depth)*1000000;
	if (blackNeighbours == 6)	return INT_MAX - 1 - (MINIMAX_DEPTH-depth)*1000000;

	int whiteScore = 0;
	whiteScore += blackNeighbours * POINTS_QUEEN_SURROUNDING;
	whiteScore -= whiteNeighbours * POINTS_QUEEN_SURROUNDING_OWN;
	whiteScore += (11 - tilesUnplayed[WHITE].size()) * POINTS_PIECES_ON_THE_BOARD;

	// BLACK EVAL

	int blackScore = 0;
	blackScore -= whiteNeighbours * POINTS_QUEEN_SURROUNDING;
	blackScore += blackNeighbours * POINTS_QUEEN_SURROUNDING_OWN;
	blackScore -= (11 - tilesUnplayed[BLACK].size()) * POINTS_PIECES_ON_THE_BOARD;

	int finalScore = whiteScore + blackScore;

	for (auto x : tilesPlayed) {
		if (x.second->willBreak())
			continue;

		char color = pieceColor(x.first);
		if (color == 'w')
			finalScore += x.second->getHeuristicValue();
		else
			finalScore -= x.second->getHeuristicValue();
	}

	return finalScore;
}

void Hive::computerMove(SocketServer& server, playerMove_t move) {

	recentMoves[translatePlayer(whoMoves)][3] = recentMoves[translatePlayer(whoMoves)][2];
	recentMoves[translatePlayer(whoMoves)][2] = recentMoves[translatePlayer(whoMoves)][1];
	recentMoves[translatePlayer(whoMoves)][1] = recentMoves[translatePlayer(whoMoves)][0];
	recentMoves[translatePlayer(whoMoves)][0] = move;

	if (!isTileInGame(move.first)) {
		placeTileSocket(whoMoves, move.first, move.second);
	}
	else {
		moveTileSocket(whoMoves, move.first, move.second);
	}

	updateHash(move.first, move.second);

	bool isOver = isGameOver();
	if (!isOver)
		nextTurn();

	server.Send(buildMovesJSON_AI(isOver, move));
}

void Hive::playerMoveSocket(SocketServer &server, bool AI) {
	bool isOver = isGameOver();

	std::string buffer = server.Recv();
	// parsing JSON

	rapidjson::Document document;
	document.Parse(buffer.c_str());

	assert(document.HasMember("Type"));
	assert(document["Type"].IsString());


	std::string messageType = document["Type"].GetString();
	if (messageType == "Move") {
		assert(document.HasMember("Tile"));
		assert(document.HasMember("Where"));
		assert(document["Tile"].IsString());
		assert(document["Where"].IsArray());

		const rapidjson::Value& tile = document["Tile"];
		const rapidjson::Value& where = document["Where"];

		std::string movedTile = tile.GetString();
		intPair_t movedWhere;
		movedWhere.first = where[0].GetInt();
		movedWhere.second = where[1].GetInt();

		std::cout << "\nRuch gracza: " << movedTile << " -> " << coordsToNeighbour(movedWhere) << " [" << movedWhere.first << ", " << movedWhere.second << "]";

		recentMoves[translatePlayer(whoMoves)][3] = recentMoves[translatePlayer(whoMoves)][2];
		recentMoves[translatePlayer(whoMoves)][2] = recentMoves[translatePlayer(whoMoves)][1];
		recentMoves[translatePlayer(whoMoves)][1] = recentMoves[translatePlayer(whoMoves)][0];

		playerMove_t newMove;
		newMove.first = movedTile;
		newMove.second = movedWhere;
		recentMoves[translatePlayer(whoMoves)][0] = newMove;

		if (!isTileInGame(movedTile)) {
			placeTileSocket(whoMoves, movedTile, movedWhere);
		}
		else {
			moveTileSocket(whoMoves, movedTile, movedWhere);
		}

		updateHash(movedTile, movedWhere);

		isOver = isGameOver();
		if (!isOver)
			nextTurn();


		if (!AI)
			server.Send(buildMovesJSON(isOver));
	}
	else if (messageType == "Pass") {

		hadNoMoves[translatePlayer(whoMoves)] = true;

		std::cout << "\nPass";

		isOver = isGameOver();
		if (!isOver)
			nextTurn(true);

		if (!AI)
			server.Send(buildMovesJSON(isOver));
	}
	else if (messageType == "EndMatch") {
		winner = 'e';
	}
	else if (messageType == "Quit") {
		winner = 'q';
	}
}

std::string Hive::buildMovesJSON(bool isOver) {
	struct availableMoves x = getAllMoves(whoMoves);

	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.Key("Type");
	writer.String("BoardState");
	writer.Key("round");
	writer.Int(move);
	writer.Key("roundBlack");
	writer.Int(movePlayer[translatePlayer('b')]);
	writer.Key("roundWhite");
	writer.Int(movePlayer[translatePlayer('w')]);
	writer.Key("whoMoves");
	writer.String((std::string()+whoMoves).c_str());
	writer.Key("GameOver");
	writer.Bool(isOver);
	if (isOver == true) {
		writer.Key("Winner");
		writer.String((std::string() + winner).c_str());
	}

	writer.Key("moves");
	writer.StartObject();
	for (auto const& y : x.tilesPlayed) {
		writer.Key(y.first.c_str());
		writer.StartArray();
		for (int i = 0; i < y.second.size(); i++) {
			writer.StartArray();
			writer.Int(y.second[i].first);
			writer.Int(y.second[i].second);
			writer.EndArray();
		}
		writer.EndArray();
	}
	writer.EndObject();

	writer.Key("placements");
	writer.StartArray();
	for (const auto& e : x.tilesUnplayed) {
		writer.StartArray();
		writer.Int(e.first);
		writer.Int(e.second);
		writer.EndArray();
	}
	writer.EndArray();

	writer.EndObject();
	return s.GetString();
}

std::string Hive::buildMovesJSON_AI(bool isOver, playerMove_t AI_Move) {
	struct availableMoves x = getAllMoves(whoMoves);

	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.Key("Type");
	writer.String("BoardState");
	writer.Key("round");
	writer.Int(move);
	writer.Key("roundBlack");
	writer.Int(movePlayer[translatePlayer('b')]);
	writer.Key("roundWhite");
	writer.Int(movePlayer[translatePlayer('w')]);
	writer.Key("whoMoves");
	writer.String((std::string() + whoMoves).c_str());
	writer.Key("GameOver");
	writer.Bool(isOver);
	if (isOver == true) {
		writer.Key("Winner");
		writer.String((std::string() + winner).c_str());
	}

	writer.Key("moves");
	writer.StartObject();
	for (auto const& y : x.tilesPlayed) {
		writer.Key(y.first.c_str());
		writer.StartArray();
		for (int i = 0; i < y.second.size(); i++) {
			writer.StartArray();
			writer.Int(y.second[i].first);
			writer.Int(y.second[i].second);
			writer.EndArray();
		}
		writer.EndArray();
	}
	writer.EndObject();

	writer.Key("placements");
	writer.StartArray();
	for (const auto& e : x.tilesUnplayed) {
		writer.StartArray();
		writer.Int(e.first);
		writer.Int(e.second);
		writer.EndArray();
	}
	writer.EndArray();

	writer.Key("AI_Move");
	writer.StartObject();
	writer.Key("Which");
	writer.String((std::string() + AI_Move.first).c_str());
	writer.Key("Where");
	writer.StartArray();
	writer.Int(AI_Move.second.first);
	writer.Int(AI_Move.second.second);
	writer.EndArray();
	writer.EndObject();

	writer.EndObject();
	return s.GetString();
}

void Hive::updateWillBreakHive() {
	std::unordered_map<std::string, Tile*> tilesPlayedCopy(tilesPlayed);

	for (auto x : tilesPlayedCopy) {
		x.second->willBreakUpdate(willBrakeHive(x.first));
	}
}

void Hive::placeTileSocket(char player, std::string which, intPair_t where) {
	hiveBrakeTab[TileIDs[which]] = true;
	tilesPlayedCount++;
	tilesPlayed[which] = tilesUnplayed[translatePlayer(player)][which];
	tilesUnplayed[translatePlayer(player)].erase(which);
	if (move > 0)	attachTile(which, where);

	updateWillBreakHive();
}

void Hive::moveTileSocket(char player, std::string which, intPair_t where) {
	if (pieceKind(which) == 'B') {
		attachTileBeetle(which, where);
	}
	else {
		attachTile(which, where);
	}

	updateWillBreakHive();
}

void Hive::playerMove() {
	struct availableMoves x;
	if (move > 0)
		x = getAllMoves(whoMoves);

	std::cout << "\n[" << whoMoves << "] Podaj ruch:";
	std::string input;
	std::getline(std::cin, input);
	struct moveCompose moveC = ParseUserInput(input);
	std::cout << "Who: " << moveC.which << " Where: " << moveC.where << " Dir: " << moveC.direction << "\n";

	if (moveC.direction == -1) {
		std::cout << "\nInvalid move! Try again!\n";
		playerMove();
		return;
	}

	if (willBrakeHive(moveC.which)) {
		std::cout << "\nMoving this tile will break the hive!";
		playerMove();
		return;
	}

	if (moveC.direction == FIRST) {
		if (!placeTile(whoMoves, moveC.which, "", -1)) {
			playerMove();
			return;
		}
		nextTurn();
		return;
	}

	if (!isTileInGame(moveC.which)) {
		if (!placeTile(whoMoves, moveC.which, moveC.where, moveC.direction)) {
			playerMove();
			return;
		}
	}
	else {
		if (!moveTile(whoMoves, moveC.which, moveC.where, moveC.direction)) {
			playerMove();
			return;
		}
	}

	if (!isGameOver())
		nextTurn();
}

void Hive::removeTile(std::string which) {
	Tile* moveTile = tilesPlayed[which];
	for (int i = 0; i < 6; i++) {
		if (moveTile->getNeighbour(i) == "")	continue;

		// reset neighbour's neighbour
		tilesPlayed[moveTile->getNeighbour(i)]->setNeighbour(getCounterDirection(i), "");

		// set tile's neighbour
		moveTile->setNeighbour(i, "");
	}
	tilesPlayed.erase(which);
}

bool Hive::willBrakeHive(std::string which) {
	auto start = std::chrono::system_clock::now();

	if (tilesPlayedCount < 3) {
		return false;
	}

	Tile* tileToRemove = tilesPlayed[which];

	if (tileToRemove->isBeetleOnTop() == true)
		return false;

	if (tileToRemove->getOnTop() != "")
		return false;

	bool hiveBrakeTabLocal[22];
	std::copy(std::begin(hiveBrakeTab), std::end(hiveBrakeTab), std::begin(hiveBrakeTabLocal));
	hiveBrakeTabLocal[tileToRemove->getID()] = false;

	stringVector neighbours = tileToRemove->getNeighbourhood();
	Tile* firstNeighbour = NULL;

	// we start from any tile adjacent to the inspected one
	for (int i = 0; i < neighbours.size(); i++) {
		if (neighbours[i] != "") {
			firstNeighbour = tilesPlayed[neighbours[i]];
			break;
		}
	}

	// temporarily removing target tile from player tiles set
	removeTile(which);

	// performing depth-first-search that touches all the pieces connected
	exploreHive(firstNeighbour, hiveBrakeTabLocal);

	// marking the pieces that are being blocked by the beetle
	for (auto x : tilesPlayed) {
		if (x.second->getOnTop() != "")
			hiveBrakeTabLocal[x.second->getID()] = false;
	}

	// restore the tile
	tilesPlayed[which] = tileToRemove;
	attachTile(which, tileToRemove->getCoords());

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	double ttime = elapsed_seconds.count();
	// std::cout << "WillBrakeHive NEW: " << ttime << "\n";

	for (bool x : hiveBrakeTabLocal) {
		if (x == true)
			return true;
	}
	return false;
}

void Hive::exploreHive(Tile* tile, bool(&brakeTab)[22]) {
	brakeTab[tile->getID()] = false;

	stringVector neighbours = tile->getNeighbourhood();

	for (int i = 0; i < neighbours.size(); i++) {
		if (neighbours[i] != "" && brakeTab[TileIDs[neighbours[i]]] != false) {
			exploreHive(tilesPlayed[neighbours[i]], brakeTab);
		}
	}
}

/*bool Hive::willBrakeHive(std::string which) {
	auto start = std::chrono::system_clock::now();

	// doesn't seem to be needed...
	//if (!isTileInGame(which))
		//return false;

	if (tilesPlayed[which]->isBeetleOnTop() == true)
		return false;

	if (tilesPlayed[which]->getOnTop() != "")
		return false;

	Tile* tileToRemove = tilesPlayed[which];
	stringVector neighbours = tileToRemove->getNeighbourhood();
	std::set<std::string> tilesToExplore;
	std::set<std::string> tilesAlreadyExplored;
	std::set<std::string> tilesFound;

	// we start from any tile adjacent to the inspected one
	for (int i = 0; i < neighbours.size(); i++) {
		if (neighbours[i] != "") {
			tilesToExplore.emplace(neighbours[i]);
			tilesFound.emplace(neighbours[i]);
			break;

		}
	}

	// temporarily removing this tile from player tiles set
	removeTile(which);

	while (tilesToExplore.size() != 0) {
		std::string investigatedTile = *tilesToExplore.begin();
		neighbours = tilesPlayed[investigatedTile]->getNeighbourhood();
		for (int i = 0; i < neighbours.size(); i++) {
			if (neighbours[i] == "")	continue;

			if (tilesAlreadyExplored.find(neighbours[i]) == tilesAlreadyExplored.end())
				tilesToExplore.emplace(neighbours[i]);

			tilesFound.emplace(neighbours[i]);
		}

		tilesAlreadyExplored.emplace(investigatedTile);
		tilesToExplore.erase(investigatedTile);
	}

	bool missing = false;
	// checking if there are any missing ones
	// ########@@@@@@ WYSTARCZY PORÓWNAÆ ROZMIAR MAP! DO POPRAWY!
	std::map<std::string, Tile*> tilesPlayedCopy(tilesPlayed);
	std::set <std::string> ::iterator it;
	for (it = tilesFound.begin(); it != tilesFound.end(); ++it) {
		tilesPlayedCopy.erase(*it);
	}

	// removing ones that are blocked by the beetle
	for (auto it = tilesPlayedCopy.begin(); it != tilesPlayedCopy.end();) {
		if (it->second->getOnTop() != "") {
			tilesPlayedCopy.erase(it++);
		}
		else {
			++it;
		}
	}

	if (tilesPlayedCopy.size() != 0)
		missing = true;

	// restore the tile
	tilesPlayed[which] = tileToRemove;
	attachTile(which, tileToRemove->getCoords());

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	double ttime = elapsed_seconds.count();
	std::cout << "WillBrakeHive: " << ttime << "\n";
	
	return missing;
}*/

int Hive::getAmountOfNeighbours(std::string which) {
	for (auto const& x : tilesPlayed) {
		if (x.first == which) {
			Tile* topTile = tilesPlayed[getTopTile(which)];

			return topTile->getNeighourCount();
		}
	}
}

bool Hive::isGameOver() {
	// Obydwaj gracze nie maj¹ ruchów

	if (hadNoMoves[0] == true && hadNoMoves[1] == true) {
		winner = 'd';
		return true;
	}

	// powtarzanie pozycji

	if (move > 8 && recentMoves[0][0] == recentMoves[0][2] && recentMoves[0][1] == recentMoves[0][3] && recentMoves[1][0] == recentMoves[1][2] && recentMoves[1][1] == recentMoves[1][3]) {
		winner = 'd';
		return true;
	}

	// otoczona krolowa
	bool surroundedWhite = false;
	bool surroundedBlack = false;
	for (auto const& x : tilesPlayed) {
		if (x.first == "bQ1") {
			Tile* topTile = tilesPlayed[getTopTile("bQ1")];

			if (topTile->isSurrounded()) {
				winner = 'w';
				surroundedBlack = true;
				continue;
			}
		}
		if (x.first == "wQ1") {
			Tile* topTile = tilesPlayed[getTopTile("wQ1")];
			if (topTile->isSurrounded()) {
				winner = 'b';
				surroundedWhite = true;
				continue;
			}
		}
	}
	if (surroundedBlack == true && surroundedWhite == true) {
		winner = 'd';
		return true;
	}

	if (surroundedBlack == true || surroundedWhite == true)
		return true;

	return false;
}

struct moveCompose Hive::ParseUserInput(std::string input) {
	struct moveCompose moveC;
	if (!std::regex_match(input, std::regex("^(b|w)(Q|S|G|B|A)[1-5]\\.(NN|NW|SW|SS|SE|NE)\\.(b|w)(Q|S|G|B|A)[1-5]$|^(b|w)(Q|S|G|B|A)[1-5]$|^(b|w)B[1-2]\\.ON\\.(b|w)(Q|S|G|B|A)[1-5]$"))) {
		moveC.direction = -1;
		return moveC;
	}
	if (input.length() == 3) {
		moveC.which = input;
		moveC.direction = FIRST;
		return moveC;
	}
	moveC.which = input.substr(0, 3);
	moveC.where = input.substr(7, 3);
	std::string dir = input.substr(4, 2);
	if (dir == "NN")
		moveC.direction = NN;
	else if (dir == "NE")
		moveC.direction = NE;
	else if (dir == "SE")
		moveC.direction = SE;
	else if (dir == "SS")
		moveC.direction = SS;
	else if (dir == "SW")
		moveC.direction = SW;
	else if (dir == "NW")
		moveC.direction = NW;
	else
		moveC.direction = ON;

	return moveC;
}

char Hive::getWinner() {
	return winner;
}

std::string Hive::buildName(char color, char bug, int number) {
	std::string str = "";
	str += color;
	str += bug;
	str += digits[number];
	return str;
}

intPair_t Hive::translateToCoords(Tile* tile, int direction) {
	intPair_t newPos = tile->getCoords();
	switch (direction) {
	case NN:
		newPos.second += 2;
		break;
	case SS:
		newPos.second -= 2;
		break;
	case NW:
		newPos.first -= 1;
		newPos.second += 1;
		break;
	case NE:
		newPos.first += 1;
		newPos.second += 1;
		break;
	case SW:
		newPos.first -= 1;
		newPos.second -= 1;
		break;
	case SE:
		newPos.first += 1;
		newPos.second -= 1;
		break;
	}
	return newPos;
}

bool Hive::placeTile(char player, std::string which, std::string where, int direction) {
	if (!IsValidTurn(player, which))
		return false;


	if (where == "" && direction == -1) {
		if (tilesPlayed.size() > 0) {
			std::cout << "\nInvalid first tile move!";
			return false;
		}
		tilesPlayed[which] = tilesUnplayed[translatePlayer(player)][which];
		tilesUnplayed[translatePlayer(player)].erase(which);
		tilesPlayed[which]->setCoords(std::pair<int, int>(0, 0));
		return true;
	}

	if (pieceColor(where) != pieceColor(which) && move != 1) {
		std::cout << "\nCannot touch opponent's tiles when placing...";
		return false;
	}

	if (isTileInGame(which) || !isTileInGame(where))
		return false;

	intPair_t destCoords = translateToCoords(tilesPlayed[where], direction);

	// Check if not attempting to attach to opponent
	if (move != 1) {
		for (auto const& x : tilesPlayed) {
			if (x.second->neighbourByCoords(destCoords) != -1 && pieceColor(x.first) != pieceColor(which) && getTopColor(x.first) != pieceColor(which)) {
				std::cout << "\nCannot touch opponent's tiles when placing...";
				return false;
			}
		}
	}

	tilesPlayed[which] = tilesUnplayed[translatePlayer(player)][which];
	tilesUnplayed[translatePlayer(player)].erase(which);
	tilesPlayed[which]->setCoords(destCoords);
	attachTile(which, destCoords);

	return true;
}

bool Hive::moveTile(char player, std::string which, std::string where, int direction) {
	if (!IsValidTurn(player, which)) {
		std::cout << "\nInvalid move!";
		return false;
	}

	if (move < 8) {
		std::cout << "\nCannot move till fourth round!";
		return false;
	}

	if (!isTileInGame(which) || !isTileInGame(where)) {
		std::cout << "\nSome tile is not in game!";
		return false;
	}

	if (tilesPlayed[which]->getOnTop() != "") {
		std::cout << "\nThis tile is blocked by the beetle!";
		return false;
	}

	intPair_t destCoords = translateToCoords(tilesPlayed[where], direction);

	if (!IsValidMove(player, which, destCoords)) {
		std::cout << "\nInvalid move!";
		return false;
	}

	if (pieceKind(which) == 'B') {
		attachTileBeetle(which, destCoords);
	}
	else {
		attachTile(which, destCoords);
	}

	return true;
}

void Hive::attachTile(std::string which, intPair_t coords) {
	// first, detachment
	Tile* moveTile = tilesPlayed[which];
	for (int i = 0; i < 6; i++) {
		if (moveTile->getNeighbour(i) == "")	continue;

		// reset neighbour's neighbour
		tilesPlayed[moveTile->getNeighbour(i)]->setNeighbour(getCounterDirection(i), "");

		// set tile's neighbour
		moveTile->setNeighbour(i, "");
	}


	// second, attachment
	moveTile->setCoords(coords);

	for (auto const& x : tilesPlayed) {
		if ((x.second)->getOnTop() != "")	continue;

		int tileDirection = (x.second)->neighbourByCoords(coords);
		if (tileDirection != -1) {
			(x.second)->setNeighbour(tileDirection, which);
			moveTile->setNeighbour(getCounterDirection(tileDirection), x.first);
		}
	}
}

void Hive::attachTileBeetle(std::string which, intPair_t coords) {
	// first, detachment
	Tile* moveTile = tilesPlayed[which];

	Tile* beetleOnTopOf = NULL;

	if (moveTile->isBeetleOnTop() == false) {
		for (int i = 0; i < 6; i++) {
			if (moveTile->getNeighbour(i) == "")	continue;

			// reset neighbour's neighbour
			tilesPlayed[moveTile->getNeighbour(i)]->setNeighbour(getCounterDirection(i), "");

			// set tile's neighbour
			moveTile->setNeighbour(i, "");
		}
	}
	else {
		for (auto const& x : tilesPlayed) {
			if (x.second->getOnTop() == which) {
				beetleOnTopOf = x.second;
				break;
			}
		}
		beetleOnTopOf->setOnTop("");
		std::string tileName = beetleOnTopOf->getName();
		stringVector beetleNeighbourhood = moveTile->getNeighbourhood();
		for (int i = 0; i < 6; i++) {
			beetleOnTopOf->setNeighbour(i, beetleNeighbourhood[i]);
			moveTile->setNeighbour(i, "");

			if (beetleNeighbourhood[i] != "")
				tilesPlayed[beetleNeighbourhood[i]]->setNeighbour(getCounterDirection(i), tileName); //
		}
	}


	// second, attachment
	// determining if the beetle jumps on top of someone
	bool onTop = false;
	for (auto const& x : tilesPlayed) { // add 'continue' if beetle on top...!!!!!
		if (x.second->getCoords() == coords) {
			// rekurencja TOP ON TOP ON TOP...
			onTop = true;
			beetleOnTopOf = tilesPlayed[getTopTile(x.second->getName())];
			break;
		}
	}
	moveTile->setCoords(coords);
	moveTile->setBeetleOnTop(onTop);

	for (auto const& x : tilesPlayed) {
		if ((x.second)->getOnTop() != "")	continue;

		int tileDirection = (x.second)->neighbourByCoords(coords);
		if (tileDirection != -1) {
			(x.second)->setNeighbour(tileDirection, which);
			moveTile->setNeighbour(getCounterDirection(tileDirection), x.first);
		}
	}
	if (onTop)
		beetleOnTopOf->setOnTop(which);
	
}

void Hive::nextTurn(bool skipped) {
	move++;

	if (skipped == false)
		movePlayer[translatePlayer(whoMoves)]++;

	whoMoves = whoMoves == 'w' ? 'b' : 'w';
	hadNoMoves[translatePlayer(whoMoves)] = false;
}

bool Hive::IsValidTurn(char player, std::string which) {
	// Checking which turn is it
	if (player != whoMoves) {
		std::cout << "\nWrong player move!";
		return false;
	}

	// Cannot move tiles that does not belong to the player
	if (pieceColor(which) != player) {
		std::cout << "It's not your tile!";
		return false;
	}

	// the Queen cannot move in the first turn
	if ((move == 0 || move == 1) && pieceKind(which) == 'Q') {
		std::cout << "\nQueen cannot move in the first move!";
		return false;
	}

	// the Queen has to be placed by turn no. 4
	if (move == 6 || move == 7) {
		if (player == 'w' && pieceKind(which) != 'Q' && !isTileInGame("wQ1")) {
			std::cout << "\nQueen has to be placed by round 4!";
			return false;
		}
		if (player == 'b' && pieceKind(which) != 'Q' && !isTileInGame("bQ1")) {
			std::cout << "\nQueen has to be placed by round 4!";
			return false;
		}
	}

	return true;
}

bool Hive::IsValidMove(char player, std::string which, intPair_t coords) {
	pairVector possibleMoves = getAvailableMoves(which);
	for (int i = 0; i < possibleMoves.size(); i++) {
		if (possibleMoves[i] == coords)
			return true;
	}

	std::cout << "\nThis tile cannot move there!";
	return false;
}



pairSet Hive::getNeighbourFields(char player) {
	pairSet mySet;

	for (auto const &it : this->tilesPlayed ) {
		if (pieceColor(it.first) != player || it.second->getOnTop() != "")	continue;
		pairVector vecCoords = it.second->getNeighbourhoodCoords();
		stringVector vecName = it.second->getNeighbourhood();
		for (int i = 0; i < 6; i++) {
			if (vecName[i] == "") {
				mySet.emplace(vecCoords[i]);
			}
		}

	}
	return mySet;
}

// TODO: 4th move, when only the queen can be pushed (perhaps some extra JSON fields?)
struct availableMoves Hive::getAllMoves(char player) {
	auto start = std::chrono::system_clock::now();
	struct availableMoves movesPlayer;


	// when dealing with the very first move we can only place the very center tile
	if (move == 0) {
		movesPlayer.tilesUnplayed.insert(std::pair<int, int>(0, 0));
		return movesPlayer;
	}

	// condition break-down: tiles aren't allowed to move till move no. 8 unless the queen is already on the board
	//if ((player == 'b' && (move >= 8 || (move < 8 && isTileInGame("bQ1")))) || (player == 'w' && (move >= 8 || (move < 8 && isTileInGame("wQ1"))))) {
	if (movePlayer[translatePlayer(player)] > 3 || (movePlayer[translatePlayer(player)]) <= 3 && isTileInGame(whoMoves  == 'b' ? "bQ1" : "wQ1")) {
		std::unordered_map<std::string, Tile*> tilesPlayedCopy(tilesPlayed);
		for(std::unordered_map<std::string, Tile *>::iterator it = tilesPlayedCopy.begin();it != tilesPlayedCopy.end(); it++)
		{
			std::string tileName = it->first;
			if (pieceColor(tileName) == player && !it->second->willBreak() && !(tilesPlayed[tileName]->getOnTop() != "")) {
				movesPlayer.tilesPlayed[tileName] = getAvailableMoves(tileName);
			}
		}
	}

	if (tilesUnplayed[translatePlayer(player)].size() > 0) {

		pairSet whitePlaces = getNeighbourFields('w');
		pairSet blackPlaces = getNeighbourFields('b');
		if (player == 'w') {
			if (move == 1) {
				movesPlayer.tilesUnplayed = blackPlaces;
			}
			else {
				for (const auto& pair : blackPlaces) {
					whitePlaces.erase(pair);
				}
				movesPlayer.tilesUnplayed = whitePlaces;
			}
		}
		else {
			if (move == 1) {
				movesPlayer.tilesUnplayed = whitePlaces;
			}
			else {
				for (const auto& pair : whitePlaces) {
					blackPlaces.erase(pair);
				}
				movesPlayer.tilesUnplayed = blackPlaces;
			}
		}
	}

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	double ttime = elapsed_seconds.count();

	return movesPlayer;
}

pairVector Hive::getAvailableMoves(std::string which) {
	pairVector vec = getQueenMoves(which);
	switch (pieceKind(which)) {
	case 'Q':
		vec = getQueenMoves(which);
		break;
	case 'S':
		vec = getSpiderMoves(which);
		break;
	case 'A':
		vec = getAntMoves(which);
		break;
	case 'G':
		vec = getGrasshopperMoves(which);
		break;
	case 'B':
		vec = getBeetleMoves(which);
		break;
	}
	return vec;
}

pairVector Hive::getQueenMoves(std::string which) {
	return getFlipMoves(which);
}

pairVector Hive::getSpiderMoves(std::string which) {
	std::pair<int, int> spiderOriginalPosition = tilesPlayed[which]->getCoords();

	intPairSet visited;
	intPairSet firstStep;
	intPairSet secondStep;
	intPairSet thirdStep;
	pairVector temp;

	visited.insert(spiderOriginalPosition);

	temp = getFlipMoves(which);
	std::copy(temp.begin(), temp.end(), std::inserter(firstStep, firstStep.end()));
	visited.insert(firstStep.begin(), firstStep.end());

	for (auto x : firstStep) {
		attachTile(which, x);
		temp = getFlipMoves(which);
		std::copy(temp.begin(), temp.end(), std::inserter(secondStep, secondStep.end()));
	}
	// secondStep.erase(visited.begin(), visited.end());
	removeFromSet(secondStep, visited);
	visited.insert(secondStep.begin(), secondStep.end());

	for (auto x : secondStep) {
		attachTile(which, x);
		temp = getFlipMoves(which);
		std::copy(temp.begin(), temp.end(), std::inserter(thirdStep, thirdStep.end()));
	}
	// thirdStep.erase(visited.begin(), visited.end());
	removeFromSet(thirdStep, visited);
	temp.clear();
	temp.assign(thirdStep.begin(), thirdStep.end());

	attachTile(which, spiderOriginalPosition);

	return temp;
}


pairVector Hive::getGrasshopperMoves(std::string which) {
	pairVector pos;

	std::string currentTile = "";
	std::string nextTile = "";

	for (int i = 0; i < 6; i++) {
		currentTile = "";
		nextTile = tilesPlayed[which]->getNeighbour(i);
		while (nextTile != "") {
			currentTile = nextTile;
			nextTile = tilesPlayed[currentTile]->getNeighbour(i);
		}

		if (currentTile != "") {
			pairVector currentTileCoords = tilesPlayed[currentTile]->getNeighbourhoodCoords();
			pos.push_back(currentTileCoords[i]);
		}
	}
	return pos;
}
pairVector Hive::getAntMoves(std::string which) {
	std::pair<int, int> antOriginalPosition = tilesPlayed[which]->getCoords();

	intPairSet visited;
	intPairSet currentStep;
	intPairSet nextStep;
	pairVector temp;

	visited.insert(antOriginalPosition);


	temp = getFlipMoves(which);
	std::copy(temp.begin(), temp.end(), std::inserter(currentStep, currentStep.end()));
	visited.insert(currentStep.begin(), currentStep.end());

	while (true) {
		nextStep.clear();

		for (auto x : currentStep) {
			attachTile(which, x);
			temp = getFlipMoves(which);
			std::copy(temp.begin(), temp.end(), std::inserter(nextStep, nextStep.end()));
		}
		removeFromSet(nextStep, visited);
		if (nextStep.size() == 0)	break;

		currentStep = nextStep;

		visited.insert(nextStep.begin(), nextStep.end());
	}

	temp.assign(visited.begin(), visited.end());

	for (int i = 0; i < temp.size(); i++) {
		if (temp[i] == antOriginalPosition) {
			temp.erase(temp.begin() + i);
			break;
		}
	}

	attachTile(which, antOriginalPosition);

	return temp;
}

pairVector Hive::getBeetleMoves(std::string which) {
	// is beetle is at the top - it can move in any direction in its surrounding
	if (tilesPlayed[which]->isBeetleOnTop()) {
		return tilesPlayed[which]->getNeighbourhoodCoords();
	}

	// if the beetle is not on top of someone it can move as a Queen + on top of adjacent tiles
	pairVector beetleMoves = getFlipMoves(which);
	stringVector beetleNeighbourhood = tilesPlayed[which]->getNeighbourhood();
	for (int i = 0; i < 6; i++) {
		if (beetleNeighbourhood[i] != "")
			beetleMoves.push_back(tilesPlayed[beetleNeighbourhood[i]]->getCoords());
	}

	return beetleMoves;
}

pairVector Hive::getFlipMoves(std::string which) {
	pairVector vec;

	stringVector neighbours = tilesPlayed[which]->getNeighbourhood();
	for (int i = 0; i < 6; i++) {
		if (neighbours[i] != "")
			continue;

		if (isVoid(neighbours[getRounded(i - 1)]) == isVoid(neighbours[getRounded(i + 1)]))
			continue;

		vec.push_back(translateToCoords(tilesPlayed[which], i));
	}

	return vec;
}

char Hive::getTopColor(std::string which) {
	std::string topTile = which;
	int color = pieceColor(topTile);
	while ((topTile = tilesPlayed[topTile]->getOnTop()) != "") {
		color = pieceColor(topTile);
	}
	return color;
}

std::string Hive::getTopTile(std::string which) {
	std::string topTile = which;
	std::string tempTile;

	while ((tempTile = tilesPlayed[topTile]->getOnTop()) != "") {
		topTile = tempTile;
	}

	return topTile;
}

bool Hive::isVoid(std::string str) {
	return str == "";
}

int Hive::getRounded(int x) {
	if (x < 0)
		return (x + 6);
	if (x > 5)
		return (x - 6);
	return x;
}

char Hive::pieceColor(std::string str) {
	return str[0];
}
char Hive::pieceKind(std::string str) {
	return str[1];
}

bool Hive::isTileInGame(std::string str) {
	return tilesPlayed.find(str) != tilesPlayed.end();
}

bool Hive::isTileUnplayed(char player, std::string str) {
	int p = translatePlayer(player);
	return tilesUnplayed[p].find(str) != tilesUnplayed[p].end();
}

int Hive::translatePlayer(char p) {
	if (p == 'w')   return WHITE;
	return BLACK;
}

int Hive::getCounterDirection(int direction) {
	return (direction + 3) % 6;
}

char Hive::getWhoMoves() {
	return whoMoves;
}

std::map<std::string, Tile*> Hive::getTilesUnplayed(int player) {
	std::map<std::string, Tile*> copyMap;
	copyMap.insert(tilesUnplayed[player].begin(), tilesUnplayed[player].end());
	return copyMap;
}


void Hive::removeFromSet(intPairSet& vec, intPairSet toPrune) {
	auto it = vec.begin();

	while (it != vec.end()) {
		bool wasErased = false;
		auto it2 = toPrune.begin();
		while (it2 != toPrune.end()) {
			if (it == vec.end())
				break;
			if (*it == *it2) {
				it = vec.erase(it);
				it2 = toPrune.erase(it2);
				wasErased = true;
			}
			else {
				++it2;
			}
		}
		if (!wasErased)	++it;
	}
}

int Hive::getPlayerMove(int color) {
	return movePlayer[color];
}