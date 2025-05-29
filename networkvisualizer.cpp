#include "networkvisualizer.h"
#include <QRandomGenerator>
#include "layerblockitem.h"
#include "colorthememanager.h"


NetworkVisualizer::NetworkVisualizer(QWidget* parent)
    : QGraphicsView(parent), m_scene(new QGraphicsScene(this)) {
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
}

QGraphicsItemGroup* NetworkVisualizer::createDetailedLayer(

    const QString& layerName,
    const QString& activation,
    int yPos
    ) {
    const int width = 160;
    const int height = 130;
    const int x = 100;
    const ColorTheme& theme = ColorThemeManager::currentTheme();
    QGraphicsItemGroup* group = new QGraphicsItemGroup();

    // 背景框
    QGraphicsRectItem* bg = new QGraphicsRectItem(0, 0, width, height);
    bg->setBrush(theme.neuronFill);//QColor(240, 240, 240)
    bg->setPen(QPen(theme.text));//QPen(Qt::blue)
    group->addToGroup(bg);

    // 层标签
    QGraphicsTextItem* title = new QGraphicsTextItem(layerName);
    title->setPos(40, 5);
    group->addToGroup(title);

    // 图形项
    QGraphicsRectItem* w = nullptr;
    QGraphicsRectItem* b = nullptr;
    QGraphicsEllipseItem* plus = nullptr;
    QGraphicsRectItem* act = nullptr;

    if (layerName != "input" && layerName != "output") {
        w = new QGraphicsRectItem(0, 0, 30, 30);
        w->setBrush(theme.weightBoxFill);//Qt::cyan
        w->setPos(10, 30);
        group->addToGroup(w);
        QGraphicsTextItem* wLabel = new QGraphicsTextItem("W", w);
        wLabel->setPos(8, 5);

        b = new QGraphicsRectItem(0, 0, 30, 30);
        b->setBrush(theme.weightBoxFill);//cyan
        b->setPos(width - 40, 30);
        group->addToGroup(b);
        QGraphicsTextItem* bLabel = new QGraphicsTextItem("b", b);
        bLabel->setPos(8, 5);

        plus = new QGraphicsEllipseItem(0, 0, 20, 20);
        plus->setBrush(theme.neuronFill);//Qt::white
        plus->setPos(width / 2 - 10, 60);
        group->addToGroup(plus);
        QGraphicsTextItem* plusLabel = new QGraphicsTextItem("+", plus);
        plusLabel->setPos(2, 0);
    }

    if (!activation.trimmed().isEmpty()) {
        act = new QGraphicsRectItem(0, 0, 100, 26);
        act->setBrush(theme.activationBoxFill);//QColor(180, 220, 255)
        act->setPos(30, 90);
        group->addToGroup(act);
        QGraphicsTextItem* actLabel = new QGraphicsTextItem(activation, act);
        actLabel->setPos(10, 5);
    }
    if (w) w->setZValue(1);
    if (b) b->setZValue(1);
    if (plus) plus->setZValue(1);
    if (act) act->setZValue(1);


    // 连线（局部坐标系内连接）
    if (w && b && plus) {
        QPointF wCenter = w->pos() + QPointF(w->rect().width() / 2, w->rect().height() / 2);
        QPointF bCenter = b->pos() + QPointF(b->rect().width() / 2, b->rect().height() / 2);
        QPointF plusCenter = plus->pos() + QPointF(plus->rect().width() / 2, plus->rect().height() / 2);

        QPen pen(theme.connectionHighWeight);//Qt::black
        pen.setWidth(2);
        QGraphicsLineItem* lineW = new QGraphicsLineItem(QLineF(wCenter, plusCenter));
        lineW->setPen(pen);
        lineW->setZValue(0);
        group->addToGroup(lineW);

        QGraphicsLineItem* lineB = new QGraphicsLineItem(QLineF(bCenter, plusCenter));
        lineB->setPen(pen);
        lineB->setZValue(0);
        group->addToGroup(lineB);
    }

    if (plus && act) {
        QPointF plusCenter = plus->pos() + QPointF(plus->rect().width() / 2, plus->rect().height() / 2);
        QPointF actTopCenter = act->pos() + QPointF(act->rect().width() / 2, 0);

        QPen pen(theme.connectionHighWeight);//Qt::black
        pen.setWidth(2);
        QGraphicsLineItem* lineToAct = new QGraphicsLineItem(QLineF(plusCenter, actTopCenter));
        lineToAct->setPen(pen);
        lineToAct->setZValue(0);
        group->addToGroup(lineToAct);
    }




    group->setPos(x, yPos);  // 整体移动
    m_scene->addItem(group);
    //group->setZValue(0); // 在上层显示

    return group;
}

void NetworkVisualizer::createNetwork(const QList<NeuralLayer>& layers) {
    m_scene->clear();
    m_allNeurons.clear();
    QVector<QVector<NeuronItem*>> allNeurons;

    const int xSpacing = 200;
    const int ySpacing = 60;

    for (int i = 0; i < layers.size(); ++i) {
        QVector<NeuronItem*> layerNeurons;
        const NeuralLayer& layer = layers[i];
        int yOffset = -(layer.neurons - 1) * ySpacing / 2;

        // 🔹 层前缀标识
        QString prefix;
        if (i == 0) prefix = "I";
        else if (i == layers.size() - 1) prefix = "O";
        else prefix = "H";

        // 🔹 添加层标签文本
        QString label = QString("%1\n(%2)").arg(layer.layerType).arg(layer.activationFunction);
        QGraphicsTextItem* layerLabel = m_scene->addText(label);
        layerLabel->setDefaultTextColor(Qt::darkBlue);
        layerLabel->setPos(i * xSpacing - 30, yOffset - 60);

        // 🔹 添加神经元
        for (int j = 0; j < layer.neurons; ++j) {
            NeuronItem* neuron = new NeuronItem(QString("%1%2").arg(prefix).arg(j + 1));
            neuron->updateColors();
            m_scene->addItem(neuron);
            neuron->setPos(i * xSpacing, yOffset + j * ySpacing);
            layerNeurons.append(neuron);
        }

        allNeurons.append(layerNeurons);// 改为存储到成员变量
        m_allNeurons = allNeurons;
    }

    // 🔹 添加连接线
    for (int i = 0; i < allNeurons.size() - 1; ++i) {
        for (NeuronItem* from : allNeurons[i]) {
            for (NeuronItem* to : allNeurons[i + 1]) {
                double weight = QRandomGenerator::global()->bounded(1.0);
                ConnectionItem* conn = new ConnectionItem(from->pos(), to->pos(), weight);
                m_scene->addItem(conn);
            }
        }
    }
}
void NetworkVisualizer::createblockNetwork(const QList<NeuralLayer>& layers) {
    m_scene->clear();
    m_layerGroups.clear();

    const int layerSpacing = 150;
    QList<QGraphicsItemGroup*> layerGroups;

    for (int i = 0; i < layers.size(); ++i) {
        const NeuralLayer& layer = layers[i];

        QString layerTitle = QString("%1").arg(layer.layerType);
        QGraphicsItemGroup* group = createDetailedLayer(
            layerTitle,
            layer.activationFunction,
            20 + i * layerSpacing
            );
        layerGroups.append(group);
        m_layerGroups.append(group);
    }

    // 连接线
    for (int i = 0; i < layerGroups.size() - 1; ++i) {
        auto from = layerGroups[i];
        auto to = layerGroups[i + 1];

        QPointF p1 = from->sceneBoundingRect().center();
        p1.setY(from->sceneBoundingRect().bottom());

        QPointF p2 = to->sceneBoundingRect().center();
        p2.setY(to->sceneBoundingRect().top());

        m_scene->addLine(QLineF(p1, p2), QPen(Qt::black));
    }

}
// networkvisualizer.cpp
void NetworkVisualizer::applyColorTheme(const QString& themeName) {
        ColorThemeManager::setCurrentTheme(themeName);
        const ColorTheme& theme = ColorThemeManager::currentTheme();

        // 1. 更新层组颜色（合并为单次遍历）
        for (QGraphicsItemGroup* group : m_layerGroups) {
            for (QGraphicsItem* item : group->childItems()) {
                if (auto* rect = dynamic_cast<QGraphicsRectItem*>(item)) {
                    // 层背景框
                    if (rect->rect().width() == 160 && rect->rect().height() == 130) {
                        rect->setBrush(theme.layerBackground);
                        rect->setPen(QPen(theme.neuronBorder, 1));
                    }
                    // 权重/偏置框
                    else if (rect->rect().width() == 30 && rect->rect().height() == 30) {
                        rect->setBrush(theme.weightBoxFill);
                    }
                    // 激活函数框
                    else if (rect->rect().width() == 100 && rect->rect().height() == 26) {
                        rect->setBrush(theme.activationBoxFill);
                    }
                }
                // 文本颜色
                else if (auto* text = dynamic_cast<QGraphicsTextItem*>(item)) {
                    text->setDefaultTextColor(theme.text);
                }
            }
        }

        // 2. 更新神经元颜色
        for (auto& layer : m_allNeurons) {
            for (NeuronItem* neuron : layer) {
                neuron->updateColors();
            }
        }

        // 3. 更新连接线颜色
        for (QGraphicsItem* item : scene()->items()) {
            if (auto* conn = dynamic_cast<ConnectionItem*>(item)) {
                conn->updateColor();
            }
        }
    }





