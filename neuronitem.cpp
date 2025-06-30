#include "neuronitem.h"
#include "colorthememanager.h"  // 假设已实现全局颜色管理器
#include <QBrush>
#include <QPen>

NeuronItem::NeuronItem(const QString& label, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent) {
    // 初始化图形和文本
    setRect(-10, -10, 20, 20);  // 保持原有大小
    m_label = new QGraphicsTextItem(label, this);
    m_label->setPos(-15, -30);  // 保持文本位置

    // 应用初始颜色
    updateColors();
    setZValue(1);  // 保持原有层级
}

void NeuronItem::updateColors() {
    // 从全局主题管理器获取颜色
    const ColorTheme& theme = ColorThemeManager::currentTheme();

    // 设置神经元颜色
    setBrush(QBrush(theme.neuronFill));
    setPen(QPen(theme.neuronBorder, 1));  // 边框宽度设为1

    // 设置文本颜色
    m_label->setDefaultTextColor(theme.text);
}


