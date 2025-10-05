#include "TopologyScene.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QtMath>
#include "../core/TopologyGenerator.h"
#include "../core/Params.h"
#include "NodeItem.h"
#include "EdgeItem.h"

static QVector<QPointF> regularPolygon(int sides, qreal radius) {
    QVector<QPointF> pts;
    pts.reserve(sides);
    const qreal angleStep = 2 * M_PI / sides;
    for (int i = 0; i < sides; ++i) {
        const qreal a = -M_PI_2 + i * angleStep; // start at top
        pts.append(QPointF(radius * qCos(a), radius * qSin(a)));
    }
    return pts;
}

static QVector<QPointF> concaveStar(const QVector<QPointF> &outer, qreal insetRatio) {
    // Build a simple star by inserting inward points between outer points
    const int n = outer.size();
    QVector<QPointF> pts;
    pts.reserve(n * 2);
    for (int i = 0; i < n; ++i) {
        const QPointF a = outer[i];
        const QPointF b = outer[(i + 1) % n];
        pts.append(a);
        const QPointF mid = (a + b) * 0.5;
        const QPointF inward = mid * insetRatio; // toward origin
        pts.append(inward);
    }
    return pts;
}

TopologyScene::TopologyScene(QObject *parent) : QGraphicsScene(parent) {
    setSceneRect(-400, -300, 800, 600);
}

void TopologyScene::updatePreview(const Params& params) {
    clear();

    std::vector<Node> nodes;
    std::vector<Edge> edges;
    TopologyGenerator::generate(params, nodes, edges);

    // Create NodeItems
    std::vector<NodeItem*> items(nodes.size(), nullptr);
    for (size_t i = 0; i < nodes.size(); ++i) {
        NodeItem *ni = new NodeItem(nodes[i].position);
        addItem(ni);
        items[i] = ni;
    }
    // Create EdgeItems and link
    for (const Edge &e : edges) {
        if (e.fromId >= 0 && e.toId >= 0 && e.fromId < (int)items.size() && e.toId < (int)items.size()) {
            EdgeItem *edgeItem = new EdgeItem(items[e.fromId], items[e.toId]);
            addItem(edgeItem);
            items[e.fromId]->addEdge(edgeItem);
            items[e.toId]->addEdge(edgeItem);
        }
    }
}

void TopologyScene::updatePreview(const ParamsLayered& params) {
    clear();

    std::vector<Node> nodes;
    std::vector<Edge> edges;
    TopologyGenerator::generateLayeredPolygons(params, nodes, edges);

    // Create NodeItems
    std::vector<NodeItem*> items(nodes.size(), nullptr);
    for (size_t i = 0; i < nodes.size(); ++i) {
        NodeItem *ni = new NodeItem(nodes[i].position);
        addItem(ni);
        items[i] = ni;
    }
    // Create EdgeItems and link
    for (const Edge &e : edges) {
        if (e.fromId >= 0 && e.toId >= 0 && e.fromId < (int)items.size() && e.toId < (int)items.size()) {
            EdgeItem *edgeItem = new EdgeItem(items[e.fromId], items[e.toId]);
            addItem(edgeItem);
            items[e.fromId]->addEdge(edgeItem);
            items[e.toId]->addEdge(edgeItem);
        }
    }
}


