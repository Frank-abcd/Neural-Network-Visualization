#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include "neuronitem.h"
#include "connectionitem.h"
#include "backend.h"
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>

/*
struct NeuralLayer {
    QString layerType; // 新增：类型（如 Dense）
    int inputSize;     // 输入维度
    int neurons;       // 神经元数量
    QString activationFunction; // 激活函数
};*/

class NetworkVisualizer : public QGraphicsView {
    Q_OBJECT
public:
    NetworkVisualizer(QWidget* parent = nullptr);
    void createNetwork(const QList<NeuralLayer>& layers);
    void createNetwork(const QJsonArray& layersJson);
    void setLayers(const QList<NeuralLayer>& layers);
    void createblockNetwork(const QList<NeuralLayer>& layers);
    void applyColorTheme(const QString& themeName);
    QGraphicsItemGroup* createDetailedLayer(const QString& layerName, const QString& activation, int yPos);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    QGraphicsScene* m_scene;
    QList<NeuralLayer> m_layers;
    QGraphicsItem* m_dragItem = nullptr;
    QPointF m_dragStartPos;
     QVector<QVector<NeuronItem*>> m_allNeurons; // 存储神经元指针以便更新
    QList<QGraphicsItemGroup*> m_layerGroups;
};
