#pragma once

#include <QPointF>
#include <vector>

struct Node {
    int id = -1;
    QPointF position;
};

struct Edge {
    int fromId = -1;
    int toId = -1;
};


