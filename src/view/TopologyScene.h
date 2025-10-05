#pragma once

#include <QGraphicsScene>

class TopologyScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit TopologyScene(QObject *parent = nullptr);

    void updatePreview(const struct Params& params);
    void updatePreview(const struct ParamsLayered& params);
};


