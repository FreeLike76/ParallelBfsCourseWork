#include "stdafx.h"

std::vector<std::vector<int>> graphGen(int size);
void sequentialBFS(std::vector<std::vector<int>> graph, int from, int to);


int main()
{
	auto graph = graphGen(250);
	sequentialBFS(graph, 0, 213);
	std::cout << "End\n";
}

std::vector<std::vector<int>> graphGen(int size)
{
	// Adjacency matrix
	std::vector<std::vector<int>> graph(size, std::vector<int>(size));

	// Making the graph connected
	for (int i = 0; i < size - 1; i++)
	{
		graph[i][i + 1] = 1;
		graph[i + 1][i] = 1;
	}

	return graph;
}

void sequentialBFS(std::vector<std::vector<int>> graph, int from, int to)
{
	std::vector<bool> visited(graph.size(), false);
	std::queue<int> queue;

	// Starting with [from]
	visited[from] = true;
	queue.push(from);

	while (!queue.empty())
	{
		// Get the first in queue
		int cur = queue.front();
		std::cout << cur << " ";
		queue.pop();
		if (cur == to) 
		{
			std::cout << "\nFOUND!\n";
			break;
		}

		// For every adjacent that is not yet visited - add to queue
		for (int i = 0; i < graph[cur].size(); i++)
		{
			if (graph[cur][i] == 1 && !visited[i])
			{
				visited[i] = true;
				queue.push(i);
			}
		}
	}
}

