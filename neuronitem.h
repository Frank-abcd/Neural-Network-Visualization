#pragma once
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QBrush>

class NeuronItem : public QGraphicsEllipseItem {
public:
    NeuronItem(const QString& label, QGraphicsItem* parent = nullptr);
};
