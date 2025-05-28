#include "connectionitem.h"
#include <QPen>

ConnectionItem::ConnectionItem(const QPointF& from, const QPointF& to, double weight) {
    setLine(QLineF(from, to));//创建一条从from到to的线段。
    QPen pen;//设置画笔样式
    pen.setColor(weight > 0.5 ? Qt::darkCyan : Qt::blue);
    pen.setWidthF(0.1 + weight * 1.9); // 线宽随权重变化
    setPen(pen);
    setZValue(0);
}
