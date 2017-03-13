#include <time.h>
#include <cmath>
#include <vector>

#include "monte_carlo.h"
#include "Node.h"
#include "Judge.h"

using namespace std;

const double EPS = 1e-6;

void Delete_board(int **&Board, int *&Top, const int M){
	delete[]Top;
	for (int i = 0; i < M; ++i)
		delete[]Board[i];
	delete[]Board;
}

inline int otherSide(int Side)
{
	return Side == 1 ? 2 : 1;
}

inline double cal_UCB(Node *node)//需要计算UCB值的节点node，蒙特卡洛搜索树访问的总次数n
{
	return (double)node->profit / node->visit_time + 1 * sqrt(2 * log(EPS + (double)node->parent->visit_time) / node->visit_time);
}

inline double cal_Vrate(Node *node)
{
	return (double)node->profit / node->visit_time;
}

//若无子节点且该节点胜负已分则返回（-2， -2），若子节点都评估过则返回（-1， -1）,
//若存在未评估的子节点（有子节点 或 无子节点但胜负未分）则返回待评估点的坐标
Point Not_Judge_Child(Node *node, int *top, const int N)
{
	int size = node->Child.size();
	if (size == 0 && node->winner != -1)  //无子节点且该节点胜负已分则返回（-2， -2）
		return Point(-2, -2);

	int column = size == 0 ? 0 : node->Child[size - 1]->y + 1;
	while (column < N){
		if (top[column] == 0)
			++column;
		else
			break;
	} 
	if (column < N)
		return Point(top[column] - 1, column);
	return Point(-1, -1);
}

void updateBoard(int x, int y, int goChessSide, const int noX, const int noY, int *Top, int **Board)
{
	Board[x][y] = goChessSide;
	if (noY == y && noX == x - 1)	Top[y] = noX;
	else	Top[y] = x;
}

void backup(int winner, Node *node)
{
	while (node != NULL){
		if (node->ChessSide == winner)
			node->profit += 1;
		else if (winner == 0)
			node->profit += 0.5;
		node = node->parent;
	}
}

void boardBack(int **Board, int **board, int *Top, const int *top, const int M, const int N)
{
	for (int i = 0; i < M; ++i)
		memcpy(Board[i], board[i], N * sizeof(int));
	memcpy(Top, top, N * sizeof(int));
}

void init_cangoColumn(vector<int> &cangoColumn, int *Top, const int N)
{
	cangoColumn.clear();
	for (int i = 0; i < N; ++i)
	if (Top[i] != 0)	cangoColumn.push_back(i);
}

void Delete_cangoColumn(vector<int> &cangoColumn, int column)
{
	for (vector<int>::iterator it = cangoColumn.begin(); it != cangoColumn.end(); it++)
	if (*it == column){
		cangoColumn.erase(it);
		return;
	}
}

Node *max_UCB_node(Node *node)//返回node的UCB值最大的孩子，调用前应保证有孩子
{	
	int choose = -1, size = node->Child.size();
	double max_UCB = -1, UCB;

	for (int i = 0; i < size; ++i){
		UCB = cal_UCB(node->Child[i]);
		if (UCB > max_UCB){
			max_UCB = UCB;
			choose = i;
		}
	}
	return node->Child[choose];
}

Node *max_Vrate_node(Node *node)
{
	int choose, size = node->Child.size();
	double max_Vrate = -1, Vrate;

	for (int i = 0; i < size; ++i){
		Vrate = cal_Vrate(node->Child[i]);
		if (Vrate > max_Vrate){
			max_Vrate = Vrate;
			choose = i;
		}
	}
	return node->Child[choose];
}

//从根节点开始模拟
void MonteCarlo_evaluate(Node *&root, const int &M, const int &N, const int &noX, const int &noY, int *Top, const int *top, int **Board, int **board, vector<int> &cangoColumn)
{
	root->visit_time++;
	Node *currentNode = root;
	Point p = Not_Judge_Child(currentNode, Top, N);

	while (p.x == -1){//currentNode的所有子节点均已评估
		currentNode = max_UCB_node(currentNode);
		currentNode->visit_time++;
		updateBoard(currentNode->x, currentNode->y, currentNode->ChessSide, noX, noY, Top, Board);
		p = Not_Judge_Child(currentNode, Top, N);
	}

	if (p.x == -2){//currentNode没有孩子节点 且 currentNode已经分出胜负
		backup(currentNode->winner, currentNode);
		boardBack(Board, board, Top, top, M, N);
		return;
	}
	//p为currentNode 的一个未评估节点
	int x = p.x, y = p.y, winner, goChessSide = otherSide(currentNode->ChessSide);
	init_cangoColumn(cangoColumn, Top, N);
	int step_num = 0;

	while (true){//开始模拟
		++step_num;
		updateBoard(x, y, goChessSide, noX, noY, Top, Board);
		if (Top[y] == 0) Delete_cangoColumn(cangoColumn, y);
		if (goChessSide == 2 ? machineWin(x, y, M, N, Board) : userWin(x, y, M, N, Board)){
			winner = goChessSide;
			break;
		}
		else if (isTie(N, Top)){
			winner = 0;
			break;
		}
		y = cangoColumn[rand() % cangoColumn.size()];
		x = Top[y] - 1;
		goChessSide = otherSide(goChessSide);
	}

	Node *nodeEval = new Node(currentNode, p.x, p.y, otherSide(currentNode->ChessSide));

	currentNode->Child.push_back(nodeEval);

	if (step_num == 1) nodeEval->winner = winner;
	nodeEval->visit_time++;
	backup(winner, nodeEval);
	boardBack(Board, board, Top, top, M, N);

}

Point* monte_carlo_getPoint(const int M, const int N, const int* top, int** board,
	const int lastX, const int lastY, const int noX, const int noY)
{
	Node *root = new Node(NULL, lastX, lastY, 1);
	Node *currentNode = root;

	int *Top = new int[N];
	memcpy(Top, top, N * sizeof(int));
	int** Board = new int*[M];
	for (int i = 0; i < M; i++){
		Board[i] = new int[N];
		memcpy(Board[i], board[i], N * sizeof(int));
	}

	vector<int> cangoColumn(N);
	srand((unsigned)time(NULL));
	clock_t start = clock();

	while (clock() - start < 4.5 * CLOCKS_PER_SEC)
	{
		MonteCarlo_evaluate(root, M, N, noX, noY, Top, top, Board, board, cangoColumn);
	}
	
	Node *go = max_Vrate_node(root);
	
	Point *p = new Point(go->x, go->y);
	Delete_board(Board, Top, M);
	delete root;

	return p;
}
