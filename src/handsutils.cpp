#include "handutils.h"

/*double hand_percent (Card a, Card b) {
	return top[a.rank()][b.rank()]/(double)(NRANKS*NRANKS);
}*/

double
handPercent (Card a, Card b) {
	int h = handToNumeric(a,b);
	return top[h/13][h%13]/(double)(NRANKS*NRANKS);
}

double
handPercentTableCoords (int i, int j) {
	return top[i][j]/(double)(NRANKS*NRANKS);
}

int
handToNumeric (Card a, Card b){
	int M = std::max(a.rank(),b.rank());
	int m = std::min(a.rank(),b.rank());
	if (a.suit() == b.suit())
		return m*NRANKS +  M;
	else
		return M*NRANKS + m;
}
