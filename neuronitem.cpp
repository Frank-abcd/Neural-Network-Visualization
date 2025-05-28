#include "neuronitem.h"

NeuronItem::NeuronItem(const QString& label, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent) {
    setRect(-10, -10, 20, 20);
    setBrush(QBrush(QColor("#AEDCF0")));
    setZValue(1);
    QGraphicsTextItem* text = new QGraphicsTextItem(label, this);
    text->setPos(-15, -30);
}
