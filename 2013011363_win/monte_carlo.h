#ifndef MONTE_CARLO_H_
#define MONTE_CARLO_H_
#include "Point.h"
Point* monte_carlo_getPoint(const int M, const int N, const int* top, int** board,
	const int lastX, const int lastY, const int noX, const int noY);




#endif // !MONTE_CARLO_H_
