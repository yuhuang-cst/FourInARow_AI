#include "Node.h"
#include <iostream>
using namespace std;

inline int otherSide(int Side)
{
	return Side == 1 ? 2 : 1;
}
Node::Node(Node *parentf, int xf, int yf, int ChessSidef)
{
	Child.clear();
	parent = parentf;
	x = xf;
	y = yf;
	ChessSide = ChessSidef;
	winner = -1; 
	visit_time = 1;
	profit = 0;
}

Node::Node(){
	parent = NULL;
	winner = -1;
	visit_time = 1;
	profit = 0;
}

Node::~Node()
{
	int size = Child.size();
	for (int i = 0; i < size; ++i)
		delete Child[i];
}