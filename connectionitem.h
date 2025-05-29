#pragma once
#include <QGraphicsLineItem>
#include "colorgroup.h"


class ConnectionItem : public QGraphicsLineItem {
public:
    ConnectionItem(const QPointF& from, const QPointF& to, double weight);
    void setColor(const QColor& color);

private:
    QColor m_color;
};
// connectionitem.h
