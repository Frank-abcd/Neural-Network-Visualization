#include "connectionitem.h"
#include <QPen>
#include "colorgroup.h"

ConnectionItem::ConnectionItem(const QPointF& from, const QPointF& to, double weight) {
    setLine(QLineF(from, to));

    QPen pen;
    // 通过静态方法获取当前主题
    const ColorTheme& theme = ColorGroup::current();
    pen.setColor(weight > 0.5 ? theme.connectionHighWeight : theme.connectionLowWeight);
    pen.setWidthF(0.1 + weight * 1.9);
    setPen(pen);
    setZValue(0);
}
