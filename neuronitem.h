#ifndef NEURONITEM_H
#define NEURONITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>

class NeuronItem : public QGraphicsEllipseItem {
public:
    NeuronItem(const QString& label, QGraphicsItem* parent = nullptr);
    void updateColors();  // 根据当前主题更新颜色

private:
    QGraphicsTextItem* m_label;
};

#endif // NEURONITEM_H
