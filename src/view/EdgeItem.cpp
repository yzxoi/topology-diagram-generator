#include "EdgeItem.h"
#include "NodeItem.h"
#include <QPen>

EdgeItem::EdgeItem(NodeItem *a, NodeItem *b, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), m_a(a), m_b(b) {
    setPen(QPen(Qt::darkBlue, 2.0));
    setZValue(0);
    adjust();
}

void EdgeItem::adjust() {
    if (!m_a || !m_b) return;
    const QPointF p1 = m_a->scenePos();
    const QPointF p2 = m_b->scenePos();
    setLine(QLineF(p1, p2));
}


