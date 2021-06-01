#include "headers.h"

char digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
char bugs[] = { 'Q', 'S', 'B', 'G', 'A' };
char bugs3[] = { 'G', 'A' };
char bugs2[] = { 'S', 'B' };

int bugs3HeuristicValue[] = { POINTS_GRASSHOPPER_VALUE, POINTS_ANT_VALUE };
int bugs2HeuristicValue[] = { POINTS_SPIDER_VALUE, POINTS_BEETLE_VALUE };

char players[] = { 'w', 'b' };

double tableTime = 0.0;

std::unordered_map<std::string, int> TileIDs;