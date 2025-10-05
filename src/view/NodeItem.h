#pragma once

#include <QGraphicsEllipseItem>
#include <vector>

class EdgeItem;

class NodeItem : public QGraphicsEllipseItem {
public:
    explicit NodeItem(const QPointF &pos, QGraphicsItem *parent = nullptr);
    void addEdge(EdgeItem *edge);
    void removeAllEdges();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    std::vector<EdgeItem*> m_edges;
};


