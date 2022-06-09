#include "Pathfinding.h"

void sequentialBFS(std::vector<std::vector<int>>& graph, std::vector<Node>& nodeVector, int start, int goal)
{
	std::queue<int> nodeQueue;

	// Starting with [from]
	nodeVector[start].d = 0;
	nodeQueue.push(start);

	while (!nodeQueue.empty())
	{
		// Get the first in queue
		int curNode = nodeQueue.front();
		nodeQueue.pop();

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
				nodeQueue.push(i);
			}
		}
	}
}

void parallelBFS(std::vector<std::vector<int>>& graph, std::vector<Node>& nodeVector, int start, int goal, int nThreads)
{
	ThreadSafeQueue<int> nodeQueue;
	bool goalIsReached = false;

	// Starting with [from]
	nodeVector[start].d = 0;
	nodeQueue.push(start);

	while (!nodeQueue.empty())
	{
		// Get the depth of a layer
		int level = nodeVector[nodeQueue.front()].d;

		// Init a vector of threads with capacity of nThreads
		std::vector<std::thread> threads;
		threads.reserve(nThreads);

		for (int i = 0; i < nThreads; i++)
		{
			// threads are avaliable but queue is empty => break
			if (nodeQueue.empty())
			{
				break;
			}
			// goal is reached => return after threads are joined
			int next = nodeQueue.front();
			if (next == goal)
			{
				goalIsReached = true;
				break;
			}
			// threads are avaliable but the node in queue is from the next layer => break
			if (nodeVector[next].d != level)
			{
				break;
			}
			// else launch a thread
			nodeQueue.pop();
			std::thread th(parallelBFSWorker, next, std::ref(graph[next]), std::ref(nodeQueue), std::ref(nodeVector));
			threads.push_back(move(th));
		}
		for (auto& th : threads)
		{
			th.join();
		}
		if (goalIsReached)
		{
			return;
		}
	}
}

void parallelBFSWorker(int curNode, std::vector<int>& adjacent, ThreadSafeQueue<int>& nodeQueue, std::vector<Node>& nodeVector)
{
	for (int i = 0; i < adjacent.size(); i++)
	{
		if (adjacent[i] == 1 && nodeVector[i].d == -1)
		{
			nodeVector[i].from = curNode;
			nodeVector[i].d = nodeVector[curNode].d + 1;
			nodeQueue.push(i);
		}
	}
}