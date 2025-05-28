#pragma once
#include <QGraphicsLineItem>

class ConnectionItem : public QGraphicsLineItem {
public:
    ConnectionItem(const QPointF& from, const QPointF& to, double weight);
};
