#include "stdafx.h"
#include "Node.h"
#include "ThreadSafeQueue.h"

std::vector<std::vector<int>> graphGen(int size, int additionalEdges = 0);
void printNodeVector(std::vector<Node>& nodeVector, int v);

void sequentialBFS(std::vector<std::vector<int>> graph, std::vector<Node>& nodeVector, int from, int goal);

void parallelBFS(std::vector<std::vector<int>> graph, std::vector<Node>& nodeVector, int start, int goal, int threads = 2);
void parallelBFSWorker(int curNode, std::vector<int>& adjacent, ThreadSafeQueue<int>& nodeQueue, std::vector<Node>& nodeVector);

int main()
{
	std::srand(std::time(NULL));

	bool writeResult = true;
	std::fstream file;

	if (writeResult)
	{
		file.open("test1.txt", std::ios::out);
		std::cout
			<< "Save results: true" << std::endl
			<< "file is opened: " << file.is_open() << std::endl;
	}
	// If file was opened write a header
	if (file.is_open())
	{
		file << "iGraphSize, iGraphPow,  iThreads, seqTime, seqDistance, parTime, parDistance\n";
	}
	for (int iGraphSize = 8; iGraphSize < 16385; iGraphSize *= 2)
	{
		for (int iGraphPow = 0; iGraphPow < 17; iGraphPow++)
		{
			for (int iThreads = 2; iThreads < 17; iThreads += 2)
			{
				for (int iRepeat = 0; iRepeat < 5; iRepeat++)
				{
					////////////////////// TASK ///////////////////////
					auto graph = graphGen(iGraphSize, iGraphPow);
					int start = 0;
					int goal = graph.size() - 1;

					std::vector<Node> seqNodeVector(graph.size());
					std::vector<Node> parNodeVector(graph.size());


					/////////////////// SEQUENTIAL ////////////////////
					std::chrono::steady_clock::time_point seqBegin = std::chrono::steady_clock::now();
					sequentialBFS(graph, seqNodeVector, start, goal);
					std::chrono::steady_clock::time_point seqEnd = std::chrono::steady_clock::now();
					//stats
					auto seqTime = std::chrono::duration_cast<std::chrono::milliseconds>(seqEnd - seqBegin).count();
					std::cout << std::endl << "Sequential: " << seqTime << " ms" << std::endl;
					printNodeVector(seqNodeVector, goal);


					//////////////////// PARALLEL /////////////////////
					std::chrono::steady_clock::time_point parBegin = std::chrono::steady_clock::now();
					parallelBFS(graph, parNodeVector, start, goal, iThreads);
					std::chrono::steady_clock::time_point parEnd = std::chrono::steady_clock::now();
					// stats
					auto parTime = std::chrono::duration_cast<std::chrono::milliseconds>(parEnd - parBegin).count();
					std::cout << std::endl << "Parallel: " << parTime << " ms" << std::endl;
					printNodeVector(parNodeVector, goal);

					/// Save results
					if (file.is_open())
					{
						file << iGraphSize << ", "
							<< iGraphPow << ", "
							<< iThreads << ", "
							<< seqTime << ", "
							<< seqNodeVector[goal].d << ", "
							<< parTime << ", "
							<< parNodeVector[goal].d << std::endl;
					}
				}
			}
		}
	}
	if (file.is_open())
	{
		file.close();
	}
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