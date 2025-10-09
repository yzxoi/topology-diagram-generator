#pragma once

#include "Params.h"
#include "NodeEdge.h"
#include <vector>
#include <unordered_set>

struct pair_hash
{
	template <class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2> &p) const
	{
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);
		return h1 ^ (h2 << 1);
	}
};

class TopologyGenerator
{
public:
	static bool generate(const Params &P, std::vector<Node> &nodes, std::vector<Edge> &edges);
	static bool generateLayeredPolygons(const ParamsLayered &P, std::vector<Node> &nodes, std::vector<Edge> &edges);
	static bool generateConcentric(const Params &P, std::vector<Node> &nodes, std::vector<Edge> &edges);

private:
	using EdgeKey = std::pair<int, int>;
	static void addEdge(int from, int to, std::vector<Edge> &edges, std::unordered_set<EdgeKey, pair_hash> &existingEdges);
};