#include "stdafx.h"
#include "Node.h"
#include "ThreadSafeQueueWrapper.h"

std::vector<std::vector<int>> graphGen(int size, int additionalEdges = 0);
void printNodeVector(std::vector<Node>& nodeVector, int v);

void sequentialBFS(std::vector<std::vector<int>> graph, std::vector<Node>& nodeVector, int from, int goal);

void parallelBFS(std::vector<std::vector<int>> graph, std::vector<Node>& nodeVector, int start, int goal, int threads = 2);
void parallelBFSWorker(int curNode, std::vector<int>& adjacent, ThreadSafeQueueWrapper<int>& nodeQueue, std::vector<Node>& nodeVector);

int main()
{
	std::srand(std::time(NULL));

	auto graph = graphGen(10000, 1);
	int start = 0;
	int goal = graph.size() - 1;

	std::vector<Node> seqNodeVector(graph.size());
	std::vector<Node> parNodeVector(graph.size());

	std::chrono::steady_clock::time_point begin, end;

	std::chrono::steady_clock::time_point seqBegin = std::chrono::steady_clock::now();
	sequentialBFS(graph, seqNodeVector, start, goal);
	std::chrono::steady_clock::time_point seqEnd = std::chrono::steady_clock::now();

	std::cout << std::endl << "Sequential: " << std::chrono::duration_cast<std::chrono::milliseconds>(seqEnd - seqBegin).count() << " ms" << std::endl;
	printNodeVector(seqNodeVector, goal);

	std::chrono::steady_clock::time_point parBegin = std::chrono::steady_clock::now();
	parallelBFS(graph, parNodeVector, start, goal, 4);
	std::chrono::steady_clock::time_point parEnd = std::chrono::steady_clock::now();
	
	std::cout << std::endl << "Parallel: " << std::chrono::duration_cast<std::chrono::milliseconds>(parEnd - parBegin).count() << " ms" << std::endl;
	printNodeVector(parNodeVector, goal);
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

void parallelBFS(std::vector<std::vector<int>> graph, std::vector<Node>& nodeVector, int start, int goal, int nThreads)
{
	ThreadSafeQueueWrapper<int> nodeQueue;
	ThreadSafeQueueWrapper<int> a;

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
			// goal is reached => return
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

void parallelBFSWorker(int curNode, std::vector<int>& adjacent, ThreadSafeQueueWrapper<int>& nodeQueue, std::vector<Node>& nodeVector)
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