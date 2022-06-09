#include "stdafx.h"

#include "Pathfinding.h"

std::vector<std::vector<int>> graphGen(int size, int branching = 2, bool debug = false);
void printNodeVector(std::vector<Node>& nodeVector,int v);

void runSerial(int graphSize);
void runParallel(int graphSize, int numThreads);
void runTest(bool writeResult, std::string filename, bool printPath);


int main()
{
	std::srand(std::time(NULL));

	//runSerial(8192);
	//runParallel(8192, 8);
	runTest(false, "test.txt", true);
}

std::vector<std::vector<int>> graphGen(int size, int branching, bool debug)
{
	if (debug)
	{
		std::cout << std::endl << "graphGen::strart";
	}
	// Init adjacency matrix with zeros
	std::vector<std::vector<int>> graph(size, std::vector<int>(size));

	// Making the graph connected
	for (int i = 0; i < size - 1; i++)
	{
		graph[i][i + 1] = 1;
		graph[i + 1][i] = 1;
	}

	// Add move edges
	if (branching > 1)
	{
		for (int i = 0; i < graph.size(); i++)
		{
			for (int j = 0; j < branching - 1; j++)
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
	if (debug)
	{
		std::cout << std::endl << "graphGen::end" << std::endl;
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

void runSerial(int graphSize)
{
	auto graph = graphGen(graphSize, 2, true);
	int start = 0;
	int goal = graph.size() - 1;

	std::vector<Node> seqNodeVector(graph.size());

	std::chrono::steady_clock::time_point seqBegin = std::chrono::steady_clock::now();
	sequentialBFS(graph, seqNodeVector, start, goal);
	std::chrono::steady_clock::time_point seqEnd = std::chrono::steady_clock::now();

	auto seqTime = std::chrono::duration_cast<std::chrono::milliseconds>(seqEnd - seqBegin).count();
	std::cout << "Sequential: " << seqTime << " ms" << std::endl;
	printNodeVector(seqNodeVector, goal);
}

void runParallel(int graphSize, int numThreads)
{
	auto graph = graphGen(graphSize, 2, true);
	int start = 0;
	int goal = graph.size() - 1;

	std::vector<Node> parNodeVector(graph.size());

	std::chrono::steady_clock::time_point parBegin = std::chrono::steady_clock::now();
	parallelBFS(graph, parNodeVector, start, goal, 8);
	std::chrono::steady_clock::time_point parEnd = std::chrono::steady_clock::now();
	
	auto parTime = std::chrono::duration_cast<std::chrono::milliseconds>(parEnd - parBegin).count();
	std::cout << "Parallel: " << parTime << " ms" << std::endl;
}

void runTest(bool writeResult, std::string filename, bool printPath)
{
	std::fstream file;
	if (writeResult)
	{
		file.open(filename, std::ios::out);
		std::cout
			<< "Save results: true" << std::endl
			<< "File is opened: " << bool(file.is_open()) << std::endl;
	}
	// If file was opened write a header
	if (file.is_open())
	{
		file << "iGraphSize,iThreads,seqTime,seqDistance,parTime,parDistance\n";
	}
	for (int iRepeat = 0; iRepeat < 5; iRepeat++)
	{
		for (int iGraphSize = 128; iGraphSize < 16385; iGraphSize *= 2)
		{
			for (int iThreads = 2; iThreads < 13; iThreads += 2)
			{
				//////////////////// TASK DEF /////////////////////
				auto graph = graphGen(iGraphSize, 2, true);
				int start = 0;
				int goal = graph.size() - 1;
				// path vectors
				std::vector<Node> seqNodeVector(graph.size());
				std::vector<Node> parNodeVector(graph.size());


				/////////////////// SEQUENTIAL ////////////////////
				std::chrono::steady_clock::time_point seqBegin = std::chrono::steady_clock::now();
				sequentialBFS(graph, seqNodeVector, start, goal);
				std::chrono::steady_clock::time_point seqEnd = std::chrono::steady_clock::now();
				//stats
				auto seqTime = std::chrono::duration_cast<std::chrono::milliseconds>(seqEnd - seqBegin).count();
				std::cout << "Sequential: " << seqTime << " ms" << std::endl;
				if (printPath)
				{
					printNodeVector(seqNodeVector, goal);
					std::cout << std::endl;
				}


				//////////////////// PARALLEL /////////////////////
				std::chrono::steady_clock::time_point parBegin = std::chrono::steady_clock::now();
				parallelBFS(graph, parNodeVector, start, goal, iThreads);
				std::chrono::steady_clock::time_point parEnd = std::chrono::steady_clock::now();
				// stats
				auto parTime = std::chrono::duration_cast<std::chrono::milliseconds>(parEnd - parBegin).count();
				std::cout << "Parallel: " << parTime << " ms" << std::endl;
				if (printPath)
				{
					printNodeVector(parNodeVector, goal);
					std::cout << std::endl;
				}

				// calculate the speedup
				std::cout << "Speedup: " << double(seqTime + 1) / double(parTime + 1) << std::endl;

				/// save results if can & needed
				if (file.is_open())
				{
					file << iGraphSize << ","
						<< iThreads << ","
						<< seqTime << ","
						<< seqNodeVector[goal].d << ","
						<< parTime << ","
						<< parNodeVector[goal].d << std::endl;
				}
			}

		}
	}
	if (file.is_open())
	{
		file.close();
	}
}
