#pragma once
#include "headers.h"

void PlayPvP(SocketServer &server);
void PlayPvC(SocketServer &server, bool computerMovesFirst);
playerMoveWithEval_t AI_MiniMax(Hive &h, TranspositionTable &t, int depth, int alpha, int beta, int player, int &n);
playerMoveWithEval_t AI_MiniMaxThreaded(Hive& h, int player, int &n);
void thread_MiniMax(int id, Hive h, int player, std::vector<playerMove_t> moves, playerMoveWithEval_t* solutions, int * threadN, int indexBegin, int indexEnd);
void CountStates(Hive &h, int depth, int color, int &nn);
std::vector<playerMove_t> buildMovesVector(Hive& h, struct availableMoves moves);



/*
{
	"Type": "GameInit"
	"WhoPlays": "PvP" (or "PvC")
	"PlayerColor": "W" (or "B")
}
*/

void InitEngine(SocketServer &server, bool shouldConnect=true) {
	// Establishing connection with GUI via sockets
	if (shouldConnect)
		server.Connect();


	// Receiving basic game informations
	std::string buffer = server.Recv();
	// parsing JSON
	rapidjson::Document document;
	document.Parse(buffer.c_str());

	// Acknowledging whether the incoming message type is "GameInit"
	buffer = document["Type"].GetString();
	if (buffer != "GameInit") {
		std::cout << "\n***Socket init went wrong...***";
		return;
	}
	// Who plays first - White or Black?
	buffer = document["PlayerColor"].GetString();
	int FirstMoveColor = Hive::MOVE_BLACK;
	if (buffer == "w")
		FirstMoveColor = Hive::MOVE_WHITE;

	// Is it PvP or PvC?
	buffer = document["WhoPlays"].GetString();
	if (buffer == "PvP") {
		PlayPvP(server);
	}
	else {
		bool computerFirst = document["ComputerFirst"].GetBool();
		PlayPvC(server, computerFirst);
	}
}

void PlayPvP(SocketServer &server) {
	std::cout << "\n\nNowy mecz PvP!";
	Hive h(Hive::MOVE_WHITE);
	const char* json = "{\"Type\":\"Play\"}";
	server.Send(json);


	while (h.getWinner() == '-') {
		h.playerMoveSocket(server, false);
		//h.playerMove();
	}

	char winner = h.getWinner();

	if (winner == 'w') {
		std::cout << "\nGame over! Winner: White";
	}
	else if (winner == 'b') {
		std::cout << "\nGame over! Winner: Black";
	}
	else if (winner == 'd') {
		std::cout << "\nGame over! Remis!";
	}
	else if (winner == 'e') {
		std::cout << "\nWymuszony koniec meczu!";
		InitEngine(server, false);
	}
	else if (winner == 'q') {
		return;
	}
}

void PlayPvC(SocketServer& server, bool computerMovesFirst) {
	std::cout << "\n\nNowy mecz PvC!";

	Hive h(Hive::MOVE_WHITE);

	TranspositionTable t;

	const char* json = "{\"Type\":\"Play\"}";
	server.Send(json);

	bool computerMove = computerMovesFirst;
	int computerColor = computerMovesFirst == true ? 0 : 1;

	float timeTable[128];
	int AmountOfMoves = 0;

	while (h.getWinner() == '-') {

		if (computerMove == false)
			h.playerMoveSocket(server, true);
		else {
			int n = 0;
			int nn = 0;
			auto start = std::chrono::system_clock::now();
			
			tableTime = 0.0;
			#ifdef THREADED
				playerMoveWithEval_t AI_Move = AI_MiniMaxThreaded(h, computerColor, n);
				// AI_MiniMax(h, MINIMAX_DEPTH, INT_MIN, INT_MAX, computerColor, nn);
			#else
				// CountStates(h, MINIMAX_DEPTH, computerColor, nn);
				playerMoveWithEval_t AI_Move = AI_MiniMax(h, t, MINIMAX_DEPTH, INT_MIN, INT_MAX, computerColor, n);
				t.clear();
			#endif
			std::cout << "\n\nRuch AI: " << AI_Move.first.first << " -> " << h.coordsToNeighbour(AI_Move.first.second) << " [" << AI_Move.first.second.first << "," << AI_Move.first.second.second << "] (Heurystyka: " << AI_Move.second << ")";
			h.computerMove(server, AI_Move.first);
			/*int mm = h.getHeuristicEvaluation(true);
			std::cout << "\nEwaluacja aktualnego stanu: " << mm;*/
			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;
			double ttime = elapsed_seconds.count();
			double sps = (double)n / ttime;
			std::cout << "\nAI przejrzalo " << n << " stanow w " << ttime << "s (" << sps << "na sekunde)\n";
			timeTable[AmountOfMoves++] = ttime;
			// std::cout << "Czas poswiecony na transposition table: " << tableTime << "\n";
			// std::cout << "Przejrzano stanow: " << n << "[Threads], " << nn << "[Proces]\n";
		}
		computerMove = computerMove == true ? false : true;
		//h.playerMove();
	}

	char winner = h.getWinner();
	float sum = 0.0;
	for (int i = 0; i < AmountOfMoves; i++) {
		std::cout << "\n" << i+1 << ". " << timeTable[i];
		sum += timeTable[i];
	}

	std::cout << "\nSuma: " << sum << "\nŒredni czas: " << sum / (float)AmountOfMoves;

	if (winner == 'w') {
		std::cout << "\nGame over! Winner: White";
	}
	else if (winner == 'b') {
		std::cout << "\nGame over! Winner: Black";
	}
	else if (winner == 'd') {
		std::cout << "\nGame over! Remis!";
	}
	else if (winner == 'e') {
		std::cout << "\nWymuszony koniec meczu!";
		InitEngine(server, false);
	}
	else if (winner == 'q') {
		return;
	}
}

void CountStates(Hive& h, int depth, int color, int& nn) {
	++nn;

	if (depth == 0 || h.isGameOver()) {
		return;
	}
	struct availableMoves x = h.getAllMoves(h.getWhoMoves());

	if (h.getPlayerMove(color) == 0) {
		return;
	}


	if (color == WHITE) {
		for (const auto& x : buildMovesVector(h, x)) {
			Hive newHive = h;
			newHive.MinimaxMove(x);
			CountStates(newHive, depth - 1, BLACK, nn);

		}

		return;
	}

	for (const auto& x : buildMovesVector(h, x)) {
		Hive newHive = h;
		newHive.MinimaxMove(x);
		CountStates(newHive, depth - 1, WHITE, nn);
	}

	return;
}

// funkcja do budowania wektora obiektów typu playerMove_t - wszystkie mo¿liwe ruchy dla danego stanu

std::vector<playerMove_t> buildMovesVector(Hive &h, struct availableMoves moves) {
	std::vector<playerMove_t> vec;

	for (const auto& x : moves.tilesPlayed) {
		for (int i = 0; i < x.second.size(); i++) {
			playerMove_t move;
			move.first = x.first;
			move.second = x.second[i];
			vec.push_back(move);
		}
	}

	std::string queen = (std::string() + (h.translatePlayer(h.getWhoMoves()) == WHITE ? "w" : "b")) + (std::string() + "Q1");

	if (h.getPlayerMove(h.translatePlayer(h.getWhoMoves())) == 3 && !h.isTileInGame(queen)) {
		for (const auto& y : moves.tilesUnplayed) {
			playerMove_t move;
			move.first = queen;
			move.second = y;
			vec.push_back(move);
		}
	}
	else {
		std::vector<char> typesOfTiles;

		for (const auto& x : h.getTilesUnplayed(h.translatePlayer(h.getWhoMoves()))) {
			char tileType = x.first.at(1);
			bool found = false;
			for (int i = 0; i < typesOfTiles.size(); i++) {
				if (typesOfTiles[i] == tileType) {
					found = true;
					break;
				}

			}
			if (!found)
				typesOfTiles.push_back(tileType);
			else continue;

			for (const auto& y : moves.tilesUnplayed) {
				playerMove_t move;
				move.first = x.first;
				move.second = y;
				vec.push_back(move);
			}
		}

	}

	// If any tile has the opportunity to move to the enemy queen's neighbourhood we want this tile to be investigated first,
	// as it will likely cause a alpha-beta cut-off
	#ifdef MOVE_ORDERING
	std::string enemyQueen = queen;
	enemyQueen[0] = h.translatePlayer(h.getWhoMoves()) == WHITE ? 'b' : 'w';

	if (!h.isTileInGame(enemyQueen))
		return vec;

	playerMove_t tmp;
	pairVector queenVec = h.tilesPlayed[enemyQueen]->getNeighbourhoodCoords();
	for (int i = 0; i < vec.size(); i++) {
		for (int j = 0; j < queenVec.size(); j++) {
			if (vec[i].second == queenVec[j]) {
				tmp = vec[i];
				vec.erase(vec.begin() + i);
				vec.insert(vec.begin(), tmp);
				break;
			}
		}
	}
	#endif

	return vec;
}

playerMoveWithEval_t AI_MiniMaxThreaded(Hive& h, int player, int& n) {
	struct availableMoves moves = h.getAllMoves(h.getWhoMoves());
	std::vector<playerMove_t> movesVector = buildMovesVector(h, moves);

	if (h.getPlayerMove(player) == 0) {
		playerMove_t newMove;
		newMove.first = std::string() + (player == WHITE ? 'w' : 'b') + std::string("G1");
		newMove.second = *moves.tilesUnplayed.begin();
		playerMoveWithEval_t AI_move;
		AI_move.first = newMove;
		AI_move.second = 0;
		return AI_move;
	}


	int movesSize = movesVector.size();
	int threadsCount = THREADS_NUM;
	if (movesSize < THREADS_NUM)
		threadsCount = movesSize; // if we have fewer moves than amount of threads, we have to reduce threads amount (obviously)...

	std::thread* threads = new std::thread[threadsCount];
	int* threadN = new int[threadsCount];

	int moveIterator = 0;
	int threadCounter = 0;
	int movesPerThread = movesSize / threadsCount;
	int additionalMove = movesSize - movesPerThread*threadsCount;

	playerMoveWithEval_t* solutions = new playerMoveWithEval_t[movesSize];

	while (threadCounter < threadsCount) {
		threads[threadCounter] = std::thread(thread_MiniMax, threadCounter, h,
											 player == 0 ? 1 : 0, movesVector,
											 solutions, threadN, moveIterator,
											 moveIterator + movesPerThread + (threadCounter >= additionalMove ? 0 : 1));
		moveIterator += movesPerThread + (threadCounter >= additionalMove ? 0 : 1);
		++threadCounter;
	}

	for (int i = 0; i < threadsCount; i++) {
		threads[i].join();
	}

	// find the best move

	int bestScore = solutions[0].second;
	int bestScoreIndex = 0;

	if (player == WHITE) {
		for (int i = 1; i < movesSize; i++) {
			if (solutions[i].second > bestScore) {
				bestScore = solutions[i].second;
				bestScoreIndex = i;
			}
		}
	}
	else {
		for (int i = 1; i < movesSize; i++) {
			if (solutions[i].second < bestScore) {
				bestScore = solutions[i].second;
				bestScoreIndex = i;
			}
		}
	}

	n = 0;
	for (int i = 0; i < threadsCount; i++) {
		n += threadN[i];
	}

	playerMoveWithEval_t AI_move;
	AI_move.first = movesVector[bestScoreIndex];
	AI_move.second = solutions[bestScoreIndex].second;

	delete[] solutions;
	delete[] threads;
	delete[] threadN;

	return AI_move;
}


void thread_MiniMax(int id, Hive h, int player,
					std::vector<playerMove_t> moves, playerMoveWithEval_t* solutions,
					int* threadN, int indexBegin, int indexEnd) {
	int n = 0;
	threadN[id] = 0;
	TranspositionTable t;
	for (int i = indexBegin; i < indexEnd; i++) {
		n = 0;
		Hive newHive = h;
		newHive.MinimaxMove(moves[i]);
		auto start = std::chrono::system_clock::now();
		playerMoveWithEval_t eval = AI_MiniMax(newHive, t, MINIMAX_DEPTH-1, INT_MIN, INT_MAX, player, n);
		playerMoveWithEval_t bestMove;
		bestMove.first = moves[i];
		bestMove.second = eval.second;
		solutions[i] = bestMove;
		t.clear();
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		double ttime = elapsed_seconds.count();
		double sps = (double)n / ttime;
		// std::cout << "\n[" << id << "] Ukonczono drzewko [" << i << "]. Stanow: " << n << " w " << ttime << " (" << sps << "na sekunde)\n";
		threadN[id] += n;
	}
}

playerMoveWithEval_t AI_MiniMax(Hive& h, TranspositionTable& t, int depth, int alpha, int beta, int color, int &n) {
	++n;
	entryWithInfo e;

	#ifdef TRANSPOSITION_TABLE
	e = t.probe(h, depth, alpha, beta);
	if (e.success == true) {
		// std::cout << "Ruch z tablicy: Heurystyka: " << e.e.heuristic << ", Depth: " << e.e.depth << ", Typ: " << e.e.state << "\n";
		playerMoveWithEval_t AI_move;
		AI_move.first = h.getMinimaxMove();
		AI_move.second = e.e.heuristic;

		if (e.e.state == STATE_EXACT) {
			//if (depth)	std::cout << "Probe Exact! Depth: " << depth << "\n";
			return AI_move;
		}

		if (e.e.state == STATE_ALPHA) {
			//if (depth)	std::cout << "Probe Alpha! Depth: " << depth << "\n";
			alpha = max(e.e.heuristic, alpha);
		}
		else if (e.e.state == STATE_BETA) {
			//if (depth)	std::cout << "Probe Beta! Depth: " << depth << "\n";
			beta = min(e.e.heuristic, beta);
		}

		if (alpha >= beta) {
			//if (depth)	std::cout << "Cutoff! Depth: " << depth << "\n";
			return AI_move;
		}

	}
	#endif
	
	if (depth == 0 || h.isGameOver()) {
		playerMoveWithEval_t AI_move;
		AI_move.first = h.getMinimaxMove();
		AI_move.second = h.getHeuristicEvaluation(depth);
		#ifdef TRANSPOSITION_TABLE
		/*
		if (AI_move.second <= alpha) {
			t.insert(h, AI_move.second, depth, STATE_ALPHA, playerMove_t("", std::pair<int, int>(0, 0)));
		}
		else if (AI_move.second >= beta) {
			t.insert(h, AI_move.second, depth, STATE_BETA, playerMove_t("", std::pair<int, int>(0, 0)));
		}
		else {
			t.insert(h, AI_move.second, depth, STATE_EXACT, playerMove_t("", std::pair<int,int>(0,0)));
		}*/
		#endif
		// std::cout << "\nHeurystyka: " << AI_move.second << " (Glebokosc: " << depth << ")";
		return AI_move;
	}
	struct availableMoves x = h.getAllMoves(h.getWhoMoves());
	std::vector<playerMove_t> movesVector = buildMovesVector(h, x);

	// if there is the best move from previous evaluation in trans. table, we move it at the very beginning of the vector
	//#ifdef TRANSPOSITION_TABLE
	#if defined(TRANSPOSITION_TABLE) && defined(MOVE_ORDERING)
	if (e.e.bestMove.first != "") {
		// first, remove the old element wherever it was
		bool found = false;
		for (int i = 0; i < movesVector.size(); i++) {
			if (movesVector[i].first == e.e.bestMove.first && movesVector[i].second == e.e.bestMove.second) {
				found = true;
				movesVector.erase(movesVector.begin() + i);
				break;
			}
		}
		if (found == true) {
			// std::cout << "Nie udalo sie podmienic: " << e.e.bestMove.first;
			// next, insert the very same element at the very beginning
			movesVector.insert(movesVector.begin(), e.e.bestMove);
			//std::cout << "\nPrzeniesiono na poczatek: " << e.e.bestMove.first << " [" << e.e.bestMove.second.first << ", " << e.e.bestMove.second.second << "]";
		}
	}
	#endif

	// first move - best move is to use spider, since its the weakest pawn, that will most likely be blocked throghout the whole match
	playerMove_t newMove;
	if (h.getPlayerMove(color) == 0) {
		newMove.first = std::string() + (color == WHITE ? 'w' : 'b') + std::string("G1");
		newMove.second = *x.tilesUnplayed.begin();
		playerMoveWithEval_t AI_move;
		AI_move.first = newMove;
		AI_move.second = h.getHeuristicEvaluation(depth);
		return AI_move;
	}

	if (color == WHITE) {
		playerMoveWithEval_t bestMove;
		bestMove.second = INT_MIN;

		// #pragma omp parallel
		for (const auto& x : movesVector) {
			Hive newHive = h;
			newHive.MinimaxMove(x);
			playerMoveWithEval_t eval = AI_MiniMax(newHive, t, depth - 1, alpha, beta, BLACK, n);

			if (depth == MINIMAX_DEPTH)
				std::cout << "\n" << x.first << " -> " << h.coordsToNeighbour(x.second) << " [" << x.second.first << "," << x.second.second << "] (Heurystyka: " << eval.second << ")";

			if (bestMove.second < eval.second) {
				bestMove.first = x;
				bestMove.second = eval.second;
			}
			if (alpha < eval.second) {
				alpha = eval.second;
			}
			if (beta <= alpha)
				break;

		}

		#ifdef TRANSPOSITION_TABLE
		if (bestMove.second <= alpha) {
			t.insert(h, bestMove.second, depth, STATE_ALPHA, bestMove.first);
		}
		else {
			t.insert(h, bestMove.second, depth, STATE_EXACT, bestMove.first);
		}
		#endif	

		return bestMove;
	}

	// BLACK

	playerMoveWithEval_t bestMove;
	bestMove.second = INT_MAX;

	// #pragma omp parallel
	for (const auto& x : movesVector) {
		Hive newHive = h;
		newHive.MinimaxMove(x);
		playerMoveWithEval_t eval = AI_MiniMax(newHive, t, depth - 1, alpha, beta, WHITE, n);

		if (depth == MINIMAX_DEPTH)
		std::cout << "\n" << x.first << " -> " << h.coordsToNeighbour(x.second) << " [" << x.second.first << "," << x.second.second << "] (Heurystyka: " << eval.second << ")";

		if (bestMove.second > eval.second) {
			bestMove.first = x;
			bestMove.second = eval.second;
		}
		if (beta >= eval.second) {
			beta = eval.second;	
		}

		if (beta <= alpha)
			break;

		#ifdef TRANSPOSITION_TABLE
		if (bestMove.second >= beta) {
			t.insert(h, bestMove.second, depth, STATE_BETA, bestMove.first);
		}
		else {
			t.insert(h, bestMove.second, depth, STATE_EXACT, bestMove.first);
		}
		#endif	
	}

	return bestMove;
}