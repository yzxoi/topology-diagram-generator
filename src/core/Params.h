#pragma once

#include <vector>
#include <string>
#include "LayeredParams.h"

// Parameters for the Polar-Topology generator
struct Params
{
	unsigned int seed = 1314520;
	int numPoints = 100;
	int numRings = 5;
	double radius = 200.0;
	double jitter = 0.1;

	// Concentric mode
	bool concentricMode = false;
	std::vector<int> nSides;
	std::vector<double> r;
	double ringJitterDeg = 0.0;
	bool connectTwo = false;
	bool useRim = false;

	// Layered polygon mode
	bool layeredPolygonMode = false;
	ParamsLayered layeredParams;

	// Original mode
	int S = 0;
	double alpha = 0.0;
	int L = 0;
	int subdiv_depth = 0;
	std::vector<int> m;
	std::vector<int> k_l;
	std::vector<int> delta_l;
	std::vector<int> q_l;
	std::vector<double> gamma;

	static Params createRadialPetalPreset();
};