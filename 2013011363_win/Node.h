#include <vector>
using namespace std;
struct Node
{
	int x, y;//形成当前棋盘状态最后一步走的哪
	int ChessSide;//形成当前棋盘状态最后一步走的一方
	int winner;//当前棋盘状态是否已分胜负平,未分胜负则为-1，已分胜负则等于胜方，0平局，1用户赢，2程序赢
	int visit_time;//本节点的访问次数
	double profit;//本节点的总收益，last_ChessSide胜得1，负得-1，平局得0
	Node *parent;
	vector<Node*> Child;

	Node(Node *parentf, int xf, int yf, int ChessSidef);
	Node();
	~Node();
};