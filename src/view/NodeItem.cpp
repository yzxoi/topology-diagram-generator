#include "NodeItem.h"
#include "EdgeItem.h"
#include <QBrush>

NodeItem::NodeItem(const QPointF &pos, QGraphicsItem *parent)
	: QGraphicsEllipseItem(parent)
{
	setRect(QRectF(-4, -4, 8, 8));
	setBrush(QBrush(Qt::red));
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	setZValue(1);
	setPos(pos);
}

void NodeItem::addEdge(EdgeItem *edge)
{
	m_edges.push_back(edge);
}

void NodeItem::removeAllEdges()
{
	m_edges.clear();
}

QVariant NodeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionChange)
	{
		for (EdgeItem *e : m_edges)
		{
			if (e)
				e->adjust();
		}
	}
	return QGraphicsEllipseItem::itemChange(change, value);
}
