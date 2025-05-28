#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDrag>
#include <QMimeData>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QJsonDocument>
#include "backend.h"
#include "json_utils.h"
#include "codegenerator.h"

NetworkGraphicsView::NetworkGraphicsView(QWidget *parent) : QGraphicsView(parent) {
    setAcceptDrops(true);
}

void NetworkGraphicsView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void NetworkGraphicsView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void NetworkGraphicsView::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasText()) {
        QString layerType = event->mimeData()->text();
        QPointF scenePos = event->position().toPoint() ;
        emit layerDropped(layerType, scenePos);
        event->acceptProposedAction();
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建自定义 GraphicsView
    networkView = new NetworkGraphicsView(this);
    if (!networkView) {
        qDebug() << "Failed to create NetworkGraphicsView";
        return;
    }
    networkView->setGeometry(ui->graphicsView->geometry());
    networkScene = new QGraphicsScene(this);
    if (!networkScene) {
        qDebug() << "Failed to create QGraphicsScene";
        return;
    }
    networkView->setScene(networkScene);

    // 隐藏原始的 graphicsView 并添加新的
    ui->graphicsView->hide();
    ui->centralwidget->layout()->addWidget(networkView);

    setWindowTitle("CodeWings:Neural-Network-Visualization");

    setupIconButton(ui->user, ":/Icon/user.png");
    setupIconButton(ui->mode, ":/Icon/mode.png");
    setupIconButton(ui->generate_code, ":/Icon/code.png");
    setupIconButton(ui->generate_image, ":/Icon/image.png");
    setupIconButton(ui->history, ":/Icon/history.png");
    setupIconButton(ui->start_new, ":/Icon/new.png");
    setupIconButton(ui->previous, ":/Icon/previous.png");
    setupIconButton(ui->turnback, ":/Icon/turnback.png");
    setupIconButton(ui->save, ":/Icon/save.png");

    // 为新添加的按钮设置拖放功能
    connect(ui->denseButton, &QPushButton::pressed, [this]() { startDrag("Dense"); });
    connect(ui->conv2dButton, &QPushButton::pressed, [this]() { startDrag("Conv2d"); });
    connect(ui->lstmButton, &QPushButton::pressed, [this]() { startDrag("LSTM"); });
    connect(ui->dropoutButton, &QPushButton::pressed, [this]() { startDrag("Dropout"); });

    connect(ui->generate_code, &QPushButton::clicked, this, &MainWindow::on_generate_code_clicked);
    connect(networkView, &NetworkGraphicsView::layerDropped, this, &MainWindow::onLayerDropped);

    if (!ui->generate_code ||!networkView) {
        qDebug() << "Invalid objects for signal-slot connection";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupIconButton(QPushButton* button, const QString& iconPath, int size) {
    button->setFixedSize(size, size);
    button->setIcon(QIcon(iconPath));
    button->setIconSize(QSize(size, size));
    button->setStyleSheet("background-color:transparent");
}

void MainWindow::startDrag(const QString& layerType) {
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(layerType);
    drag->setMimeData(mimeData);
    drag->exec(Qt::MoveAction);
}

void MainWindow::onLayerDropped(const QString& layerType, const QPointF& position) {
    // 使用现有backend中的NeuralLayer工厂函数创建层
    NeuralLayer* layer = NeuralLayer::fromJsonObject(QJsonObject{{"layerType", layerType}});
    if (layer) {
        // 设置层的位置 (假设NeuralLayer有setPosition方法)
        layer->setPosition(QPointF(position.x(), position.y()));
        selectedLayers.append(layer);

        // 在场景中添加图层图形项
        QGraphicsItem* layerItem = createLayerGraphicsItem(layer);
        networkScene->addItem(layerItem);

        // 更新连接线
        updateConnections();
    }
}

// 更新连接线路
void MainWindow::updateConnections() {
    // 清除现有连接
    for (QGraphicsItem* item : connectionItems) {
        networkScene->removeItem(item);
        delete item;
    }
    connectionItems.clear();

    // 创建新连接
    for (int i = 0; i < selectedLayers.size() - 1; ++i) {
        if(!selectedLayers[i]){
        QGraphicsLineItem* line = new QGraphicsLineItem(
            selectedLayers[i]->getPosition().x() + 50,
            selectedLayers[i]->getPosition().y() + 25,
            selectedLayers[i+1]->getPosition().x(),
            selectedLayers[i+1]->getPosition().y() + 25
            );
        line->setPen(QPen(Qt::blue, 2));
        networkScene->addItem(line);
        connectionItems.append(line);
        }
        else
            qDebug()<<"selectedLayers[i] is nullptr";
    }
}

// 创建图层图形项
QGraphicsItem* MainWindow::createLayerGraphicsItem(NeuralLayer* layer) {
    // 创建容器图形项
    QGraphicsRectItem* container = new QGraphicsRectItem();
    container->setBrush(QBrush(Qt::white));
    container->setPen(QPen(Qt::black, 1));

    // 创建层类型标签
    QGraphicsTextItem* label = new QGraphicsTextItem(layer->layerType, container);
    QFont font;
    font.setPointSize(10);
    font.setBold(true);
    label->setFont(font);

    // 根据层类型设置不同样式
    QColor baseColor;
    QString details;

    if (layer->layerType == "Conv2d") {
        Conv2dLayer* convLayer = dynamic_cast<Conv2dLayer*>(layer);
        baseColor = QColor(70, 130, 180);  // 钢蓝色
        details = QString("Ch:%1→%2\nKernel:%3")
                      .arg(convLayer->inputChannels)
                      .arg(convLayer->outputChannels)
                      .arg(convLayer->kernelSize);
    }
    else if (layer->layerType == "Dense") {
        DenseLayer* denseLayer = dynamic_cast<DenseLayer*>(layer);
        baseColor = QColor(34, 139, 34);  // 森林绿
        details = QString("Neurons:%1").arg(denseLayer->neurons);
    }
    else if (layer->layerType == "MaxPool2d") {
        baseColor = QColor(218, 165, 32);  // 金色
        details = "Pooling Layer";
    }
    // 其他层类型的样式
    else if (layer->layerType == "LSTM") {
        baseColor = QColor(138, 43, 226);  // 紫罗兰
        details = "Recurrent Layer";
    }
    else if (layer->layerType == "Dropout") {
        baseColor = QColor(255, 69, 0);    // 橙红色
        details = "Regularization";
    }
    else {
        baseColor = QColor(169, 169, 169); // 灰色
        details = layer->layerType;
    }

    // 设置容器样式
    QLinearGradient gradient(0, 0, 0, 50);
    gradient.setColorAt(0, baseColor.lighter(150));
    gradient.setColorAt(1, baseColor.darker(120));
    container->setBrush(QBrush(gradient));

    // 添加详情文本
    QGraphicsTextItem* detailText = new QGraphicsTextItem(details, container);
    detailText->setFont(QFont("Arial", 8));
    detailText->setPos(0, 20);

    // 添加激活函数标签（如果有）
    if (!layer->activationFunction.isEmpty()) {
        QGraphicsTextItem* activationText = new QGraphicsTextItem(
            QString("Activation: %1").arg(layer->activationFunction),
            container
            );
        activationText->setFont(QFont("Arial", 8, QFont::StyleItalic));
        activationText->setDefaultTextColor(Qt::darkGray);
        activationText->setPos(0, 40);
    }

    // 设置容器大小
    QRectF textRect = label->boundingRect().united(detailText->boundingRect());
    container->setRect(0, 0, qMax(120.0, textRect.width() + 10), 70);

    // 居中文本
    label->setPos((container->rect().width() - label->boundingRect().width()) / 2, 5);

    // 添加连接点（顶部和底部）
    addConnectionPoint(container, container->rect().topLeft() + QPointF(container->rect().width()/2, 0), true);
    addConnectionPoint(container, container->rect().bottomLeft() + QPointF(container->rect().width()/2, 0), false);

    return container;
}

// 添加连接点辅助函数
void MainWindow::addConnectionPoint(QGraphicsItem* parent, const QPointF& pos, bool isInput) {
    QGraphicsEllipseItem* point = new QGraphicsEllipseItem(-4, -4, 8, 8, parent);
    point->setPos(pos);
    point->setBrush(isInput ? QBrush(Qt::green) : QBrush(Qt::red));
    point->setPen(QPen(Qt::black, 1));
    point->setZValue(1);  // 确保连接点在最上层
}

void MainWindow::on_generate_code_clicked()
{
    // 将层列表转换为 JSON 数组
    QJsonArray layersArray;
    for (NeuralLayer* layer : selectedLayers) {
        layersArray.append(layer->toJsonObject());
    }

    // 生成网络结构 JSON
    QString jsonData = generateNetworkStructureJson(layersArray);

    // 生成 PyTorch 代码
    QString code = CodeGenerator::generatePyTorchCode(selectedLayers);

    // 在右侧显示生成的代码 (假设使用QTextEdit显示代码)
    if (ui->codePreview) {
        ui->codePreview->setPlainText(code);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        QString text = event->mimeData()->text();
        // 这里编写放下操作后的业务逻辑代码
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}


void MainWindow::handleJsonData(const QString &jsonStr) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        // 假设obj表示网络结构，从中提取层信息
        QJsonArray layersArray = obj["layers"].toArray();
        for (const QJsonValue &layerValue : layersArray) {
            QJsonObject layerObj = layerValue.toObject();
            NeuralLayer* layer = NeuralLayer::fromJsonObject(layerObj);
            if (layer) {
                // 后续可对layer进行操作
                // 例如：selectedLayers.append(layer.release());
            }
        }
    }
}
