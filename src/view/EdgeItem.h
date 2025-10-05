#pragma once

#include <QGraphicsLineItem>

class NodeItem;

class EdgeItem : public QGraphicsLineItem {
public:
    EdgeItem(NodeItem *a, NodeItem *b, QGraphicsItem *parent = nullptr);
    void adjust();
    NodeItem* source() const { return m_a; }
    NodeItem* target() const { return m_b; }
private:
    NodeItem *m_a;
    NodeItem *m_b;
};


