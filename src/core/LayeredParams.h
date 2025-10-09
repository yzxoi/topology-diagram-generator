#pragma once

#include <vector>
#include <string>

struct LayerSpec
{
	int n;				 // Number of sides for this layer's polygon (>=3)
	double radius;		 // Radius for this layer
	double phaseDeg;	 // Starting angle for this layer (degrees)
	int subdivPerEdge;	 // Number of subdivision points per edge {0,1,2}
	double subdivOffset; // Subdivision offset [0,0.5]; =0.5 for equal division; =1/3 for three equal parts
	bool drawRim;		 // Whether to draw the rim edges for this layer
	bool drawDiagonals;	 // Optional: draw diagonals (star/skip)
	int skipK;			 // Skip step for drawDiagonals=true (0 to disable)
};

struct InterLayerRule
{
	// Rules for connecting layer l (outer) to layer l+1 (inner):
	bool connectOneToOne; // Nearest 1->1 (closest angle)
	bool connectOneToTwo; // 1->2 (connect to nearest and one neighbor)
	bool connectZigzag;	  // Alternate connecting left/right neighbors for weaving effect
};

struct ParamsLayered
{
	int L = 3;							 // Number of layers (3-4)
	std::vector<LayerSpec> layers;		 // From outer to inner
	std::vector<InterLayerRule> between; // Size L-1
	double jitterDeg = 0.0;				 // Small vertex jitter (degrees), can be 0
	unsigned seed = 1314520;
	bool connectCenter = true; // Connect center anchor to all points of the innermost layer
};
