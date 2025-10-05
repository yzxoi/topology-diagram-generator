#include "Params.h"

Params Params::createRadialPetalPreset() {
    Params p;
    p.S = 12;
    p.alpha = 0.8;
    p.L = 3;
    p.r = {100, 150, 200};
    p.m = {6, 12, 18};
    p.k_l = {1, 2, 3};
    p.delta_l = {0, 1, 0};
    p.q_l = {1, 1, 1};
    p.gamma = {0.0, 0.5, 0.25};
    p.seed = 42;
    p.concentricMode = false;
    p.layeredPolygonMode = false;
    return p;
}
