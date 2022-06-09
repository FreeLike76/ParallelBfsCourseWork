#pragma once

#include "stdafx.h"

void sequentialBFS
(
	std::vector<std::vector<int>>& graph,
	std::vector<Node>& nodeVector,
	int from,
	int goal
);
void parallelBFS
(
	std::vector<std::vector<int>>& graph,
	std::vector<Node>& nodeVector,
	int start,
	int goal,
	int threads = 2
);
void parallelBFSWorker
(
	int curNode,
	std::vector<int>& adjacent,
	ThreadSafeQueue<int>& nodeQueue,
	std::vector<Node>& nodeVector
);