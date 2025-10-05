#include "TopologyGenerator.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <QPointF>
#include <QTransform>
#include <iostream>
#include "Params.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double deg2rad(double deg)
{
	return deg * M_PI / 180.0;
}

// Helper to add an edge if it doesn't already exist
void TopologyGenerator::addEdge(int from, int to, std::vector<Edge> &edges, std::unordered_set<EdgeKey, pair_hash> &existingEdges)
{
	if (from == to)
		return;
	EdgeKey key(std::min(from, to), std::max(from, to));
	if (existingEdges.find(key) == existingEdges.end())
	{
		edges.push_back({from, to});
		existingEdges.insert(key);
	}
}

// Helper function to map an index from one ring to another based on angle
static int mapIndexByAngle(int i, int M0, int M1, double gamma0, double index_offset)
{
	double normalized_angle = (double(i) + gamma0) / M0;
	double target_index_float = normalized_angle * M1;
	int j = static_cast<int>(round(target_index_float + index_offset));
	return (j % M1 + M1) % M1;
}

static inline double norm2pi(double a)
{
	a = std::fmod(a, 2.0 * M_PI);
	if (a < 0)
		a += 2.0 * M_PI;
	return a;
}
static inline int imod(int x, int m)
{
	int r = x % m;
	return r < 0 ? r + m : r;
}

bool TopologyGenerator::generateConcentric(const Params &P, std::vector<Node> &nodes, std::vector<Edge> &edges)
{
	nodes.clear();
	edges.clear();

	std::mt19937 rng(P.seed);
	std::uniform_real_distribution<double> jitter_dist(-deg2rad(P.ringJitterDeg), deg2rad(P.ringJitterDeg));

	int id_counter = 0;
	std::vector<std::vector<int>> rings;

	for (size_t i = 0; i < P.r.size(); ++i)
	{
		if (i >= P.nSides.size())
			continue;

		double radius = P.r[i];
		int n_points = P.nSides[i];
		if (n_points <= 0)
			continue;

		std::vector<int> ring_nodes;
		for (int j = 0; j < n_points; ++j)
		{
			double angle = 2 * M_PI * j / n_points + jitter_dist(rng);
			double x = radius * cos(angle);
			double y = radius * sin(angle);
			nodes.push_back({id_counter, {x, y}});
			ring_nodes.push_back(id_counter);
			id_counter++;
		}
		rings.push_back(ring_nodes);
	}

	std::unordered_set<EdgeKey, pair_hash> existingEdges;

	for (const auto &ring : rings)
	{
		if (P.useRim && ring.size() > 1)
		{
			for (size_t i = 0; i < ring.size(); ++i)
			{
				addEdge(ring[i], ring[(i + 1) % ring.size()], edges, existingEdges);
			}
		}
	}

	if (P.connectTwo)
	{
		for (size_t i = 0; i < rings.size() - 1; ++i)
		{
			const auto &ring1 = rings[i];
			const auto &ring2 = rings[i + 1];
			if (ring1.empty() || ring2.empty())
				continue;

			for (int u_idx = 0; u_idx < ring1.size(); ++u_idx)
			{
				int u = ring1[u_idx];
				QPointF pos_u = nodes[u].position;

				std::vector<std::pair<double, int>> dists;
				for (int v_idx = 0; v_idx < ring2.size(); ++v_idx)
				{
					int v = ring2[v_idx];
					QPointF pos_v = nodes[v].position;
					dists.push_back({QPointF::dotProduct(pos_u - pos_v, pos_u - pos_v), v});
				}

				std::sort(dists.begin(), dists.end());

				addEdge(u, dists[0].second, edges, existingEdges);
				if (dists.size() > 1)
				{
					addEdge(u, dists[1].second, edges, existingEdges);
				}
			}
		}
	}

	return true;
}

// ===== 主函数：分层多边形生成 =====
bool TopologyGenerator::generateLayeredPolygons(const ParamsLayered &P,
												std::vector<Node> &nodes,
												std::vector<Edge> &edges)
{
	if (P.L <= 0 || (int)P.layers.size() != P.L)
		return false;

	nodes.clear();
	edges.clear();
	std::mt19937 rng(P.seed);
	std::uniform_real_distribution<double> uj(-deg2rad(P.jitterDeg), deg2rad(P.jitterDeg));

	std::vector<std::vector<int>> ringIds(P.L);
	std::vector<std::vector<double>> ringAng(P.L);

	ringIds.reserve(P.L);
	ringAng.reserve(P.L);

	int id = 0;
	const double EPS = 1e-6;

	for (int l = 0; l < P.L; ++l)
	{
		const auto &Ls = P.layers[l];
		int n = std::max(3, Ls.n);
		double R = std::max(EPS, Ls.radius);
		double phi = deg2rad(Ls.phaseDeg);

		ringIds[l].clear();
		ringAng[l].clear();
		ringIds[l].reserve(n * (1 + Ls.subdivPerEdge));
		ringAng[l].reserve(n * (1 + Ls.subdivPerEdge));

		std::vector<QPointF> verts(n);
		for (int i = 0; i < n; ++i)
		{
			double th = phi + 2.0 * M_PI * i / n + uj(rng);
			verts[i] = QPointF(R * std::cos(th), R * std::sin(th));
		}

		for (int i = 0; i < n; ++i)
		{
			int j = (i + 1) % n;
			QPointF A = verts[i], B = verts[j];

			auto addPoint = [&](const QPointF &Pnt)
			{
				nodes.push_back({id, Pnt});
				ringIds[l].push_back(id);
				ringAng[l].push_back(std::atan2(Pnt.y(), Pnt.x()));
				++id;
			};

			addPoint(A);

			int s = std::clamp(Ls.subdivPerEdge, 0, 2);
			double p = std::clamp(Ls.subdivOffset, 0.0, 0.5);
			if (s == 1)
			{
				QPointF M((A.x() + B.x()) * 0.5, (A.y() + B.y()) * 0.5);
				addPoint(M);
			}
			else if (s == 2)
			{
				QPointF P1(A.x() * (1.0 - p) + B.x() * p, A.y() * (1.0 - p) + B.y() * p);
				QPointF P2(A.x() * p + B.x() * (1.0 - p), A.y() * p + B.y() * (1.0 - p));
				addPoint(P1);
				addPoint(P2);
			}
		}

		if (Ls.drawRim)
		{
			for (int k = 0; k < (int)ringIds[l].size(); ++k)
			{
				int a = ringIds[l][k], b = ringIds[l][(k + 1) % ringIds[l].size()];
				edges.push_back({a, b});
			}
		}
		if (Ls.drawDiagonals && Ls.skipK > 0)
		{
			int M = (int)ringIds[l].size();
			int step = std::min(Ls.skipK, M / 2);
			for (int k = 0; k < M; ++k)
			{
				edges.push_back({ringIds[l][k], ringIds[l][(k + step) % M]});
			}
		}
	}

	std::unordered_set<EdgeKey, pair_hash> seen;
	auto add = [&](int a, int b)
	{
		if (a == b)
			return;
		EdgeKey ek{std::min(a, b), std::max(a, b)};
		if (seen.insert(ek).second)
			edges.push_back({a, b});
	};

	for (int l = 0; l < P.L - 1; ++l)
	{
		const auto &rule = P.between[l];
		const auto &outIds = ringIds[l];
		const auto &outAng = ringAng[l];
		const auto &inIds = ringIds[l + 1];
		const auto &inAng = ringAng[l + 1];

		int nIn = std::max(1, P.layers[l + 1].n);
		double phiIn = deg2rad(P.layers[l + 1].phaseDeg);

		for (int u = 0; u < (int)outIds.size(); ++u)
		{
			double th = outAng[u];
			double rel = norm2pi(th - phiIn);
			int j = (int)std::llround(rel * nIn / (2.0 * M_PI)) % nIn;
			int v0 = 0;
			double best = 1e100;
			for (int k = 0; k < (int)inIds.size(); ++k)
			{
				double d = std::abs(norm2pi(inAng[k] - th));
				d = std::min(d, 2.0 * M_PI - d);
				if (d < best)
				{
					best = d;
					v0 = k;
				}
			}
			add(outIds[u], inIds[v0]);

			if (rule.connectOneToTwo)
			{
				int dir = 1;
				if (rule.connectZigzag)
				{
					add(outIds[u], inIds[imod(v0 + dir, (int)inIds.size())]);
					dir = (u & 1) ? -1 : +1;
				}
				add(outIds[u], inIds[imod(v0 + dir, (int)inIds.size())]);
			}
		}
	}

	if (P.connectCenter && P.L > 0 && !ringIds.back().empty())
	{
		int centerId = id++;
		nodes.push_back({centerId, QPointF(0, 0)});
		for (int v : ringIds.back())
			add(centerId, v);
	}

	return true;
}

bool TopologyGenerator::generate(const Params &P, std::vector<Node> &nodes, std::vector<Edge> &edges)
{
	if (P.layeredPolygonMode)
	{
		return generateLayeredPolygons(P.layeredParams, nodes, edges);
	}
	else
	{
		return generateConcentric(P, nodes, edges);
	}
}
