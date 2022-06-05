#include "Node.h"

Node::Node()
{
	from = -1;
	d = -1;
}

Node::Node(int d, int from) 
{
	this->from = from;
	this->d = d;
}