#include "TranspositionTable.h"

void TranspositionTable::insert(Hive& h, int heuristics, int depth, int state, playerMove_t move) {
	std::pair<mapIterator_t, mapIterator_t> bucket = map.equal_range(h.hash);
	mapIterator_t it = bucket.first;
	bool found = false;

	while (it != bucket.second) {
		if (it->first != h.hash) {
			++it;
			continue;
		}
		found = true;
		// std::cout << "\n" << it->first << "\n" << h.hash << "\n\n";

		if (it->second.depth >= depth) { // > lub >=...ktore wybrac?
			it->second = entry{ heuristics, depth, state, move };
		}
		break;

	}

	if (!found) {
		mapIterator_t newEntry = map.insert({ h.hash, entry{heuristics, depth, state, move } });
	}
}

entryWithInfo TranspositionTable::probe(Hive &h, int depth, int alpha, int beta) {
	std::pair<mapIterator_t, mapIterator_t> bucket = map.equal_range(h.hash);
	mapIterator_t it = bucket.first;
	bool found = false;

	while (it != bucket.second) {
		if (it->first != h.hash) {
			++it;
			continue;
		}
		found = true;
		break;
	}

	if (!found) {
		return entryWithInfo{ NULL, false };
	}

	// at this point, we know that under 'it' we have a valid entry we're looking for


	if (it->second.depth < depth) {
		return entryWithInfo{ NULL, false };
	}

	return entryWithInfo{ it->second, true };

	/*if (it->second.state == STATE_EXACT) {
		return entryWithInfo{ it->second, true };
	}*/

	if (it->second.state == STATE_ALPHA && it->second.heuristic <= alpha) {
		return entryWithInfo{ entry{alpha, it->second.depth, STATE_ALPHA, it->second.bestMove}, true };
	}

	if (it->second.state == STATE_BETA && it->second.heuristic >= beta) {
		return entryWithInfo{ entry{beta, it->second.depth, STATE_BETA, it->second.bestMove}, true };
	}


	return entryWithInfo{ it->second, false };
}

void TranspositionTable::clear() {
	map.clear();
}