#pragma once

#include <vector>
#include <string>

struct LayerSpec
{
	int n;
	double radius;
	double phaseDeg;
	int subdivPerEdge;
	double subdivOffset;
	bool drawRim;
	bool drawDiagonals;
	int skipK;
};

struct InterLayerRule
{
	// Rules for connecting layer l (outer) to layer l+1 (inner):
	bool connectOneToOne;
	bool connectOneToTwo;
	bool connectZigzag;
};

struct ParamsLayered
{
	int L = 3;
	std::vector<LayerSpec> layers;
	std::vector<InterLayerRule> between;
	double jitterDeg = 0.0;
	unsigned seed = 1314520;
	bool connectCenter = true;
};
