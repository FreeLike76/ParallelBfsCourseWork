#include "stdafx.h"
#include "Node.h"

std::vector<std::vector<int>> graphGen(int size, int additionalEdges = 0);
void printNodeVector(std::vector<Node>& nodeVector, int v);

void sequentialBFS(std::vector<std::vector<int>> graph, std::vector<Node>& nodeVector, int from, int goal);
void parallelBFS(std::vector<std::vector<int>> graph, std::vector<Node>& nodeVector, int start, int goal, int threads=2);

int main()
{
	std::srand(std::time(NULL));

	auto graph = graphGen(1000, 1);
	int start = 0;
	int goal = graph.size() - 1;

	auto seqNodeVector = std::vector<Node>(graph.size());
	auto parNodeVector = std::vector<Node>(graph.size());

	sequentialBFS(graph, seqNodeVector, start, goal);

	printNodeVector(seqNodeVector, goal);
	std::cout << "\nThe End\n";
}

std::vector<std::vector<int>> graphGen(int size, int additionalEdges)
{
	// Init adjacency matrix with zeros
	std::vector<std::vector<int>> graph(size, std::vector<int>(size));

	// Making the graph connected
	for (int i = 0; i < size - 1; i++)
	{
		graph[i][i + 1] = 1;
		graph[i + 1][i] = 1;
	}

	// Add move edges
	if (additionalEdges > 0)
	{
		for (int i = 0; i < graph.size(); i++)
		{
			for (int j = 0; j < additionalEdges; j++)
			{
				int k;
				do
				{
					k = std::rand() % graph[i].size();
				} while (k == i + 1);
				graph[i][k] = 1;
			}
		}
	}

	return graph;
}

void printNodeVector(std::vector<Node>& nodeVector, int v)
{
	if (nodeVector[v].from != -1)
	{
		printNodeVector(nodeVector, nodeVector[v].from);
	}
	std::cout << v << ":" << nodeVector[v].d << ", ";
}

void sequentialBFS(std::vector<std::vector<int>> graph, std::vector<Node>& nodeVector, int start, int goal)
{
	std::queue<int> queue;

	// Starting with [from]
	nodeVector[start].d = 0;
	queue.push(start);

	while (!queue.empty())
	{
		// Get the first in queue
		int curNode = queue.front();
		queue.pop();

		// For every adjacent that is not yet visited - add to queue
		for (int i = 0; i < graph[curNode].size(); i++)
		{
			if (graph[curNode][i] == 1 && nodeVector[i].d == -1)
			{
				nodeVector[i].from = curNode;
				nodeVector[i].d = nodeVector[curNode].d + 1;
				if (i == goal)
				{
					return;
				}
				queue.push(i);
			}
		}
	}
}

void parallelBFS(std::vector<std::vector<int>> graph, std::vector<Node>& nodeVector, int start, int goal, int nThreads)
{
	std::queue<int> queue;

	// Starting with [from]
	nodeVector[start].d = 0;
	queue.push(start);

	while (!queue.empty())
	{
		// Get the current level
		int level = nodeVector[queue.front()].d;
		std::vector<int> currentFrontier;

		// Fill frontier vector
		while (nodeVector[queue.front()].d == level)
		{
			currentFrontier.push_back(queue.front());
			queue.pop();
		};

		std::vector<std::thread> threads(nThreads);
		//If the node is the goal
		//if (currentNode == goal)
		//{
		//	return;
		//}
		
		// FOR THREAD IN RANGE NUM_THREADS
		// IF QUEUE LEVEL IS OK
		//    START PROCCES
		// ELSE
		//   WAIT UNTIL ALL ARE FINISHED
	}
}

void parallelBFSWorker(int curNode, std::vector<int>& adjacent, std::queue<int>& queue, std::vector<Node>& nodeVector)
{
	for (int i = 0; i < adjacent.size(); i++)
	{
		if (adjacent[i] == 1 && nodeVector[i].d == -1)
		{
			nodeVector[i].from = curNode;
			nodeVector[i].d = nodeVector[curNode].d + 1;

			queue.push(i);
		}
	}
}