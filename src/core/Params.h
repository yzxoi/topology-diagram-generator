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

	static Params createRadialPetalPreset();
};
