#pragma once

#include <QGraphicsView>

class TopologyView : public QGraphicsView {
    Q_OBJECT
public:
    explicit TopologyView(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
};


