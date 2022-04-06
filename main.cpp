#include "stdafx.h"

std::vector<std::vector<int>> graphGen(int size);

int main()
{
	auto graph = graphGen(250);

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

