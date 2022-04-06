#include "stdafx.h"

std::vector<std::vector<int>> graphGen(int size, int additionalEdges = 0);
void sequentialBFS(std::vector<std::vector<int>> graph, int from, int to);


int main()
{
	std::srand(std::time(NULL));

	auto graph = graphGen(250, 5);
	sequentialBFS(graph, 0, graph.size() - 1);
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

		// For every adjacent that is not yet visited - add to queue
		for (int i = 0; i < graph[cur].size(); i++)
		{
			if (graph[cur][i] == 1 && !visited[i])
			{
				if (i == to)
				{
					return;
				}
				visited[i] = true;
				queue.push(i);
			}
		}
	}
}

