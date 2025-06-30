#pragma once
#include <QGraphicsLineItem>

class ConnectionItem : public QGraphicsLineItem {
public:
    ConnectionItem(const QPointF& from, const QPointF& to, double weight);
    void updateColor();  // 根据当前主题更新颜色
    double weight() const { return m_weight; }

private:
    double m_weight;
};
