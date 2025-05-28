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
    void setLayers(const QList<NeuralLayer>& layers);
    void createblockNetwork(const QList<NeuralLayer>& layers);
    QGraphicsItemGroup* createDetailedLayer(const QString& layerName, const QString& activation, int yPos);


private:
    QGraphicsScene* m_scene;
};
