#include "TopologyView.h"
#include <QWheelEvent>

TopologyView::TopologyView(QWidget *parent) : QGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing, true);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void TopologyView::wheelEvent(QWheelEvent *event) {
    const double factor = (event->angleDelta().y() > 0) ? 1.15 : 1.0/1.15;
    scale(factor, factor);
}


