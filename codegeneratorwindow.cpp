#include "codegeneratorwindow.h"
#include "ui_codegeneratorwindow.h"
#include "mainwindow.h"
#include "propertypanel.h"
#include "codegenerator.h"
#include <QGraphicsRectItem>
#include <QObject>
#include <QMimeData>
#include <QDrag>
#include <QGraphicsProxyWidget>
#include <QMenu>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneMoveEvent>
#include <QClipboard>
#include <QLabel>
#include <QStatusBar>
#include <QMessageBox>
#include <QTimer>

CodeGeneratorWindow::CodeGeneratorWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CodeGeneratorWindow)
    , params()
    , m_dragConnectionPoint(nullptr)
    , m_dragPath(nullptr)
    , isInCustomMode(false)
{
    ui->setupUi(this);
    // Initialize network visualizer
    m_builderScene = new QGraphicsScene(this);
    m_builderView = new QGraphicsView(m_builderScene, this);
    m_codeDisplay = new QTextEdit(this);
    m_codeDisplay->setReadOnly(true);
    m_propertyPanel = new PropertyPanel(this);

    // Set main window layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Layer selection area
    QListWidget* layersList = new QListWidget(this);
    layersList->addItem("Input Layer");
    layersList->addItem("Hidden Layer");
    layersList->addItem("Output Layer");
    layersList->addItem("Dense Layer");
    layersList->addItem("Convolutional Layer");
    layersList->addItem("MaxPooling Layer");
    layersList->addItem("AveragePooling Layer");
    layersList->addItem("LSTM Layer");
    layersList->addItem("RNN Layer");
    layersList->addItem("GRU Layer");
    layersList->addItem("Dropout Layer");

    // 创建一个容器，用于放置画布和属性面板
    QWidget* canvasAndPanelContainer = new QWidget(this);
    QHBoxLayout* innerLayout = new QHBoxLayout(canvasAndPanelContainer);
    innerLayout->addWidget(m_builderView);
    innerLayout->addWidget(m_propertyPanel);
    innerLayout->setContentsMargins(0, 0, 0, 0);

    // 创建一个滚动区域，并将容器放入其中
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(canvasAndPanelContainer);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 左侧层选择列表和滚动区域的布局
    QHBoxLayout* layersLayout = new QHBoxLayout();
    layersLayout->addWidget(layersList);
    layersLayout->addWidget(scrollArea);

    // Code display area
    mainLayout->addLayout(layersLayout);
    mainLayout->addWidget(m_codeDisplay);

    // 创建一个水平布局用于放置现有的代码显示框和复制按钮
    QHBoxLayout* codeLayout = new QHBoxLayout();
    codeLayout->addWidget(m_codeDisplay); // 使用现有的代码显示框
    // 创建复制按钮
    m_copyCodeButton = new QPushButton("复制", this);
    connect(m_copyCodeButton, &QPushButton::clicked, this, &CodeGeneratorWindow::on_copyCodeButton_clicked);
    codeLayout->addWidget(m_copyCodeButton, 0, Qt::AlignTop | Qt::AlignRight);
    mainLayout->addLayout(codeLayout);

    setLayout(mainLayout);

    // Button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);

    // Connect signals and slots
    connect(layersList, &QListWidget::itemClicked, this, &CodeGeneratorWindow::on_layersList_itemClicked);
    connect(m_propertyPanel, &PropertyPanel::parametersUpdated, this, &CodeGeneratorWindow::on_propertiesPanel_parametersUpdated);

    // Generate code button
    QPushButton* generateCodeButton = new QPushButton("Generate PyTorch Code", this);
    connect(generateCodeButton, &QPushButton::clicked, this, &CodeGeneratorWindow::on_generateCodeButton_clicked);
    buttonLayout->addWidget(generateCodeButton);
    buttonLayout->setSpacing(10);

    // Delete button
    QPushButton* deleteLayerButton = new QPushButton("Delete Selected Layer", this);
    connect(deleteLayerButton, &QPushButton::clicked, this, &CodeGeneratorWindow::deleteSelectedLayer);
    buttonLayout->addWidget(deleteLayerButton);

    // Return to main menu
    QPushButton* returnButton = new QPushButton("返回主界面", this);
    connect(returnButton, &QPushButton::clicked, this, &CodeGeneratorWindow::on_return_mainwindow_clicked);
    buttonLayout->addWidget(returnButton);

    buttonLayout->setContentsMargins(500, 0, 500, 0);
    mainLayout->addLayout(buttonLayout);

    // Enable mouse tracking
    m_builderView->setMouseTracking(true);
    setMouseTracking(true);

    // 在构造函数中添加
    connect(m_builderScene, &QGraphicsScene::selectionChanged,
            this, &CodeGeneratorWindow::onSceneSelectionChanged);

}

CodeGeneratorWindow::~CodeGeneratorWindow()
{
    delete ui;
}


void CodeGeneratorWindow::on_return_mainwindow_clicked()
{
    this->close();
    MainWindow::instance()->show();
}

void CodeGeneratorWindow::deleteSelectedLayer() {
    QList<QGraphicsItem*> selectedItems = m_builderScene->selectedItems();
    if (!selectedItems.isEmpty()) {
        QGraphicsRectItem* selectedItem = qgraphicsitem_cast<QGraphicsRectItem*>(selectedItems[0]);
        if (selectedItem) {
            // 删除所有关联的连接线图形项
            foreach (QGraphicsItem* item, m_builderScene->items()) {
                if (item->type() == QGraphicsItem::UserType + 1) { // 假设连接线是自定义类型，可根据实际类型调整
                    m_builderScene->removeItem(item);
                    delete item;
                }
            }

            // 从连接列表中移除关联的连接对
            m_connections.removeIf([selectedItem](const QPair<ConnectionPointItem*, ConnectionPointItem*>& pair) {
                return pair.first->parentItem() == selectedItem || pair.second->parentItem() == selectedItem;
            });

            //将selectedItems对应的层从m_layers中删除
            for (QGraphicsItem* selectedItem : selectedItems) {
                // 找到与选中图元对应的层
                for (int i = 0; i < m_layers.size(); ++i) {
                    if (m_layers[i].layerType == selectedItem->data(0).value<NeuralLayer>().layerType) {
                        // 从 m_layers 中移除对应的层
                        m_layers.removeAt(i);
                        break;
                    }
                }
            }

            // 删除层图形项
            m_builderScene->removeItem(selectedItem);
            delete selectedItem;
        }
    }
    else{
        // 显示短暂提示消息（3秒后自动消失）
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("Conform");
        msgBox->setText("Please select a layer");
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setStandardButtons(QMessageBox::NoButton); // 无按钮
        msgBox->show();
        // 3秒后自动关闭
        QTimer::singleShot(3000, msgBox, &QMessageBox::deleteLater);
    }
}
void CodeGeneratorWindow::on_generateCodeButton_clicked() {
    QList<NeuralLayer> layers;
    QMap<ConnectionPointItem*, NeuralLayer*> pointToLayer; // 连接点到层的映射

    // 收集所有层和连接点
    /*for (QGraphicsItem* item : m_builderScene->items()) {
        if (auto* layerItem = dynamic_cast<QGraphicsRectItem*>(item)) {
            NeuralLayer layer = layerItem->data(0).value<NeuralLayer>();
            layers.append(layer);
            // 映射层的顶部和底部连接点
            for (QGraphicsItem* child : layerItem->childItems()) {
                if (auto* point = dynamic_cast<ConnectionPointItem*>(child)) {
                    pointToLayer[point] = &layers.last();
                }
            }
        }
    }*/

    // 解析连接关系（从输出点到输入点）
    QList<NeuralLayer*> orderedLayers;
    QSet<NeuralLayer*> visited;

    for (NeuralLayer& layer : m_layers) {
        orderedLayers.append(&layer);
        visited.insert(&layer);
        qDebug()<<layer.layerType;
    }

    // 按连接关系遍历层
    /*int currentIndex = 0;
    while (currentIndex < orderedLayers.size()) {
        NeuralLayer* currentLayer = orderedLayers[currentIndex];
        // 查找当前层底部连接点的所有输出连接
        for (const auto& conn : m_connections) {
            if (conn.first->data(0).toString() == "bottom" &&
                pointToLayer.contains(conn.first) &&
                pointToLayer[conn.first] == currentLayer) {
                NeuralLayer* nextLayer = pointToLayer[conn.second];
                if (!visited.contains(nextLayer)) {
                    orderedLayers.append(nextLayer);
                    visited.insert(nextLayer);
                }
            }
        }
        currentIndex++;

    }*/

    // 生成代码
    QString code = CodeGenerator::generatePyTorchCode(orderedLayers);
    m_codeDisplay->setPlainText(code);
}


void CodeGeneratorWindow::on_layersList_itemClicked(QListWidgetItem* item) {
    if (!isInCustomMode){
        QString layerType = item->text();
        layerType.remove(" Layer");

        NeuralLayer layer;
        layer.layerType = layerType;

        QColor color = colorForLayerType(layerType);

        MyGraphicsRectItem* layerItem = new MyGraphicsRectItem();
        layerItem->setRect(0, 0, 100, 50);
        layerItem->setBrush(color);
        layerItem->setData(0, QVariant::fromValue(layer));
        m_builderScene->addItem(layerItem);

        connect(layerItem, &MyGraphicsRectItem::positionChanged, this, [this, layerItem]() {
            updateLayerConnections(layerItem);
        });


        params["LayerType"] = layerType;
        if(layerType =="Dense"){
            params["neurons"] = "10";
            params["activation"] = "ReLU";
            layer.neurons = 10;
            layer.activationFunction = "ReLU";
        }
        else if (layerType == "Convolutional") {
            params["filters"] = "32";
            params["kernelSize"] = "5";
            layer.filters = 32;
            layer.kernelSize = 5;
        }
        else if (layerType == "MaxPooling" || layerType == "AvgPooling") {
            params["poolingSize"] = "5";
            layer.poolingSize = 5;
            layer.neurons = 1;
        }
        else if (layerType == "LSTM" || layerType == "RNN" || layerType == "GRU") {
            params["units"] = "128";
        }
        else if (layerType == "Dropout") {
            params["dropoutRate"] = "0.5";
            layer.dropoutRate = 0.5f;
            layer.neurons =1;
        }
        else if (layerType == "Input") {
            params["neurons"] = "15";
            layer.neurons = 15;
        }
        else if (layerType == "Output") {
            params["neurons"] = "2";
            layer.neurons = 2;
        }
        else if (layerType == "Hidden") {
            params["neurons"] = "10";
            params["activation"] = "ReLU";
            layer.neurons = 10;
            layer.activationFunction = "ReLU";
        }
        m_layers.append(layer);

        m_propertyPanel->clearParameters();

        m_propertyPanel->setParameters(params);

        layerItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        layerItem->setAcceptDrops(true);

        QGraphicsTextItem* layerNameText = new QGraphicsTextItem(layerType, layerItem);
        layerNameText->setPos(layerItem->rect().center().x() - layerNameText->boundingRect().width() / 2,
                              layerItem->rect().center().y() - layerNameText->boundingRect().height() / 2);

        layerItem->setZValue(1);

        // Add connection points to the new layer
        // addConnectionPoints(layerItem);
    }
    else{
        m_propertyPanel->clearParameters();
        QString layerType = item->text();
        layerType.remove(" Layer");

        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData();
        mimeData->setText(layerType);
        drag->setMimeData(mimeData);

        QPixmap pixmap(100, 50);
        pixmap.fill(colorForLayerType(layerType));
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(50, 25));

        drag->exec(Qt::CopyAction);
    }
}

void CodeGeneratorWindow::on_propertiesPanel_parametersUpdated(const QMap<QString, QString>& params) {
    // 更新选中的层的参数
    QList<QGraphicsItem*> selectedItems = m_builderScene->selectedItems();
    if (!selectedItems.isEmpty()) {
        QGraphicsRectItem* selectedItem = qgraphicsitem_cast<QGraphicsRectItem*>(selectedItems[0]);
        if (selectedItem) {
            // 使用 QVariant::value() 安全地获取 NeuralLayer 对象
            QVariant data = selectedItem->data(0);
            NeuralLayer selectedLayer = data.value<NeuralLayer>();
            QString layerType = params["LayerType"];

            if (layerType == "Dense") {
                selectedLayer.neurons = params["neurons"].toInt();
                selectedLayer.activationFunction = params["activation"];
            }
            else if (layerType == "Convolutional") {
                selectedLayer.filters = params["filters"].toInt();
                selectedLayer.kernelSize = params["kernel_size"].toInt();
            }
            else if (layerType == "MaxPooling" || layerType == "AvgPooling") {
                selectedLayer.poolingSize = params["pooling_size"].toInt();
            }
            else if (layerType == "LSTM" || layerType == "RNN") {
                selectedLayer.units = params["units"].toInt();
            }
            else if (layerType == "Dropout") {
                selectedLayer.dropoutRate = params["dropout_rate"].toFloat();
            }
            else if (layerType == "Hidden") {
                selectedLayer.neurons = params["neurons"].toInt();
                selectedLayer.activationFunction = params["activation"];
            }
            else if (layerType == "Output") {
                selectedLayer.neurons = params["neurons"].toInt();
            }
            // 更新其他层类型参数...

            // 更新 m_layers 中对应层
            for (int i = 0; i < m_layers.size(); ++i) {
                if (m_layers[i].layerType == layerType) {
                    m_layers[i] = selectedLayer;
                    break;
                }
            }
        }
    }
    else{
        // 显示短暂提示消息（3秒后自动消失）
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("Conform");
        msgBox->setText("Please select a layer");
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setStandardButtons(QMessageBox::NoButton); // 无按钮
        msgBox->show();
        // 3秒后自动关闭
        QTimer::singleShot(3000, msgBox, &QMessageBox::deleteLater);
    }
}


QColor CodeGeneratorWindow::colorForLayerType(const QString& layerType) {
    if (layerType == "Input") return Qt::cyan;
    else if (layerType == "Hidden") return Qt::yellow;
    else if (layerType == "Output") return Qt::magenta;
    else if (layerType == "Convolutional") return Qt::green;
    else if (layerType == "MaxPooling") return Qt::blue;
    else if (layerType == "AvgPooling") return Qt::blue;
    else if (layerType == "LSTM") return Qt::red;
    else if (layerType == "RNN") return Qt::darkCyan;
    else if (layerType == "Dropout") return Qt::gray;
    else if (layerType == "Dense") return Qt::darkYellow;
    else return Qt::white;
}

void CodeGeneratorWindow::dropEvent(QDropEvent* event) {
    if (!isInCustomMode) {
        // 如果不在自定义模式，执行原有的代码生成功能逻辑
        return;
    }

    if (event->mimeData()->hasText()) {
        QString layerType = event->mimeData()->text();

        // 创建一个新的 NeuralLayer 对象
        NeuralLayer layer;
        layer.layerType = layerType;

        // 创建一个新的标签来表示神经网络层，并放置在工作区
        QLabel* layerLabel = new QLabel(this);
        layerLabel->setText(layerType);
        layerLabel->setFixedSize(150, 50);
        layerLabel->setAlignment(Qt::AlignCenter);
        layerLabel->setStyleSheet("background-color: white; border: 1px solid black;");
        //layerLabel->move(event->pos());
        layerLabel->move(event->position().toPoint());

        // 保存层到列表中，以便后续可以访问
        m_layers.append(layer);

        // 添加层到场景中
        QGraphicsRectItem* layerItem = new QGraphicsRectItem(0, 0, 100, 50);
        layerItem->setBrush(colorForLayerType(layerType));
        layerItem->setData(0, QVariant::fromValue(layer));
        m_builderScene->addItem(layerItem);

        // 更新连接关系（这里简单模拟连接关系）
        updateConnections();
    }
}

void CodeGeneratorWindow::updateConnections() {
    // 清除现有的连接线
    for (QGraphicsItem* item : m_builderScene->items()) {
        if (auto* conn = dynamic_cast<QGraphicsLineItem*>(item)) {
            if (conn != m_dragLine) {
                m_builderScene->removeItem(conn);
                delete conn;
            }
        }
    }
    // 重新创建连接线
    for (QPair<ConnectionPointItem*, ConnectionPointItem*> connection : m_connections) {
        ConnectionPointItem* fromPoint = connection.first;
        ConnectionPointItem* toPoint = connection.second;

        // 创建连接线
        QGraphicsLineItem* conn = new QGraphicsLineItem();
        conn->setPen(QPen(Qt::black, 2));
        conn->setLine(QLineF(fromPoint->scenePos(), toPoint->scenePos()));
        m_builderScene->addItem(conn);
    }
}

void CodeGeneratorWindow::startConnectionDrag(ConnectionPointItem* connectionPoint, QGraphicsSceneMouseEvent* event) {
    m_dragConnectionPoint = connectionPoint;

    // Create temporary curve
    m_dragPath = new QGraphicsPathItem();
    m_dragPath->setPen(QPen(Qt::gray, 2, Qt::DashLine));
    m_builderScene->addItem(m_dragPath);

    // Initialize the path with current position
    updateDragPath(event->scenePos());
}

void CodeGeneratorWindow::updateDragLine(const QPointF& pos) {
    if (m_dragConnectionPoint && m_dragLine) {
        QPointF startPoint = m_dragConnectionPoint->scenePos();
        startPoint.setY(startPoint.y() + (m_dragConnectionPoint->data(0).toString() == "top" ? 5 : -5));
        m_dragLine->setLine(QLineF(startPoint, pos));
    }
}

void CodeGeneratorWindow::endConnectionDrag(QGraphicsSceneMouseEvent* event) {
    if (m_dragPath) {
        ConnectionPointItem* targetPoint = nullptr;
        const double CONNECTION_THRESHOLD = 10.0; // 连接阈值（像素）

        QPointF scenePos = event->scenePos();

        // 查找有效连接点（距离小于阈值且非自身）
        for (QGraphicsItem* item : m_builderScene->items()) {
            auto* point = dynamic_cast<ConnectionPointItem*>(item);
            if (point && point != m_dragConnectionPoint) {
                double distance = QLineF(scenePos, point->scenePos()).length();
                if (distance < CONNECTION_THRESHOLD) {
                    targetPoint = point;
                    break; // 取最近的一个
                }
            }
        }

        if (targetPoint) {
            createConnection(m_dragConnectionPoint, targetPoint);
        } else {
            // 无效连接，删除拖拽路径
            m_builderScene->removeItem(m_dragPath);
            delete m_dragPath;
            m_dragPath = nullptr;
        }
        m_dragConnectionPoint = nullptr;
    }
}

void CodeGeneratorWindow::updateDragPath(const QPointF& pos) {
    if (m_dragConnectionPoint && m_dragPath) {
        QPointF start = m_dragConnectionPoint->scenePos() + QPointF(5, 5); // Center of the connection point
        QPainterPath path;
        path.moveTo(start);

        // Create quadratic Bezier curve
        QPointF controlPoint((start.x() + pos.x())/2, (start.y() + pos.y())/2);
        path.quadTo(controlPoint, pos);

        m_dragPath->setPath(path);
    }
}

void CodeGeneratorWindow::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragConnectionPoint) {
        QPointF scenePos = m_builderView->mapToScene(event->pos());
        updateDragPath(scenePos);
    }
    QDialog::mouseMoveEvent(event);
}

void CodeGeneratorWindow::mouseReleaseEvent(QMouseEvent* event) {
    if (m_dragConnectionPoint) {
        QPointF scenePos = m_builderView->mapToScene(event->pos());

        // Find if we're releasing over another connection point
        QGraphicsItem* item = m_builderScene->itemAt(scenePos, QTransform());
        if (item && item != m_dragConnectionPoint) {
            ConnectionPointItem* targetPoint = dynamic_cast<ConnectionPointItem*>(item);
            if (targetPoint && targetPoint->parentItem() != m_dragConnectionPoint->parentItem()) {
                createConnection(m_dragConnectionPoint, targetPoint);
            }
        }

        // Clean up temporary drag path
        if (m_dragPath) {
            m_builderScene->removeItem(m_dragPath);
            delete m_dragPath;
            m_dragPath = nullptr;
        }

        m_dragConnectionPoint = nullptr;
    }
    QDialog::mouseReleaseEvent(event);
}

void CodeGeneratorWindow::createConnection(ConnectionPointItem* fromPoint, ConnectionPointItem* toPoint) {
    // Create permanent connection curve
    QGraphicsPathItem* conn = new QGraphicsPathItem();
    conn->setPen(QPen(Qt::black, 2));

    QPointF start = fromPoint->scenePos() + QPointF(5, 5);
    QPointF end = toPoint->scenePos() + QPointF(5, 5);
    QPointF controlPoint((start.x() + end.x())/2, (start.y() + end.y())/2);

    QPainterPath path;
    path.moveTo(start);
    path.quadTo(controlPoint, end);
    conn->setPath(path);

    m_builderScene->addItem(conn);
    conn->setZValue(0); // Behind layers
    m_connections.append(qMakePair(fromPoint, toPoint));
    m_connectionItems.append(conn);
}

void CodeGeneratorWindow::updateLayerConnections(QGraphicsRectItem* layerItem) {
    for (int i = 0; i < m_connections.size(); ++i) {
        auto& pair = m_connections[i];
        auto* connItem = m_connectionItems[i];

        if (pair.first->parentItem() == layerItem || pair.second->parentItem() == layerItem) {
            QPointF start = pair.first->scenePos() + QPointF(5, 5);
            QPointF end = pair.second->scenePos() + QPointF(5, 5);
            QPointF controlPoint((start.x() + end.x())/2, (start.y() + end.y())/2);

            QPainterPath path;
            path.moveTo(start);
            path.quadTo(controlPoint, end);
            connItem->setPath(path);
        }
    }
}

void CodeGeneratorWindow::on_copyCodeButton_clicked() {
    // 获取代码显示框中的文本
    QString code = m_codeDisplay->toPlainText();
    // 获取系统剪贴板
    QClipboard* clipboard = QApplication::clipboard();
    // 将代码复制到剪贴板
    clipboard->setText(code);
}

QJsonArray CodeGeneratorWindow::getNetworkAsJson() const {
    QJsonArray array;
    for (const NeuralLayer& layer : m_layers) {
        array.append(layer.toJsonObject());
    }
    return array;
}

void CodeGeneratorWindow::clearNetwork()
{
    m_layers.clear();              // 清空网络层数据
    m_builderScene->clear();      // 清空画布
    m_codeDisplay->clear();       // 清空代码框
}

void CodeGeneratorWindow::onSceneSelectionChanged() {
    QList<QGraphicsItem*> selectedItems = m_builderScene->selectedItems();
    if (selectedItems.isEmpty()) {
        m_propertyPanel->clearParameters();
        return;
    }

    // 只处理第一个选中的项
    QGraphicsItem* selectedItem = selectedItems.first();
    NeuralLayer layer = selectedItem->data(0).value<NeuralLayer>();

    // 准备当前层的参数
    QMap<QString, QString> currentParams;
    currentParams["LayerType"] = layer.layerType;

    if (layer.layerType == "Dense" || layer.layerType == "Hidden") {
        currentParams["neurons"] = QString::number(layer.neurons);
        currentParams["activation"] = layer.activationFunction;
    } else if (layer.layerType == "Convolutional") {
        currentParams["filters"] = QString::number(layer.filters);
        currentParams["kernel_size"] = QString::number(layer.kernelSize);
    } else if (layer.layerType == "MaxPooling" || layer.layerType == "AvgPooling") {
        currentParams["pooling_size"] = QString::number(layer.poolingSize);
    } else if (layer.layerType == "LSTM" || layer.layerType == "RNN" || layer.layerType == "GRU") {
        currentParams["units"] = QString::number(layer.units);
    } else if (layer.layerType == "Dropout") {
        currentParams["dropout_rate"] = QString::number(layer.dropoutRate);
    } else if (layer.layerType == "Input" || layer.layerType == "Output") {
        currentParams["neurons"] = QString::number(layer.neurons);
    }

    // 更新属性面板（会清除旧参数）
    m_propertyPanel->setParameters(currentParams);
}
