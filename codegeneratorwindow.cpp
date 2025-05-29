#include "codegeneratorwindow.h"
#include "ui_codegeneratorwindow.h"
#include "mainwindow.h"
#include "networkvisualizer.h"
#include "propertypanel.h"
#include "codegenerator.h"
#include <QGraphicsRectItem>
#include <QMimeData>
#include <QDrag>

CodeGeneratorWindow::CodeGeneratorWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CodeGeneratorWindow)
{
    ui->setupUi(this);
    // 初始化网络可视化器
    m_builderScene = new QGraphicsScene(this);
    m_builderView = new QGraphicsView(m_builderScene, this);
    m_codeDisplay = new QTextEdit(this);
    m_codeDisplay->setReadOnly(true);
    m_propertyPanel = new PropertyPanel(this);

    // 设置主窗口布局
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // 层选择区域
    QHBoxLayout* layersLayout = new QHBoxLayout();
    QListWidget* layersList = new QListWidget(this);
    layersList->addItem("Input Layer");
    layersList->addItem("Hidden Layer");
    layersList->addItem("Output Layer");
    layersList->addItem("Dense Layer");
    layersList->addItem("Convolutional Layer");
    layersList->addItem("Pooling Layer");
    layersList->addItem("LSTM Layer");
    layersList->addItem("RNN Layer");
    layersList->addItem("Dropout Layer");
    layersLayout->addWidget(layersList);

    // 画布区域
    layersLayout->addWidget(m_builderView);

    // 属性设置面板
    layersLayout->addWidget(m_propertyPanel);

    // 代码显示区域
    mainLayout->addLayout(layersLayout);
    mainLayout->addWidget(m_codeDisplay);

    setLayout(mainLayout);

    // 连接信号和槽
    connect(layersList, &QListWidget::itemClicked, this, &CodeGeneratorWindow::on_layersList_itemClicked);
    connect(m_propertyPanel, &PropertyPanel::parametersUpdated, this, &CodeGeneratorWindow::on_propertiesPanel_parametersUpdated);

    // 生成代码按钮
    QPushButton* generateCodeButton = new QPushButton("Generate PyTorch Code", this);
    connect(generateCodeButton, &QPushButton::clicked, this, &CodeGeneratorWindow::on_generateCodeButton_clicked);
    mainLayout->addWidget(generateCodeButton);
}

CodeGeneratorWindow::~CodeGeneratorWindow()
{
    delete ui;
}

void CodeGeneratorWindow::on_layersList_itemClicked(QListWidgetItem* item) {
    QString layerType = item->text();
    layerType.remove(" Layer");

    NeuralLayer layer;
    layer.layerType = layerType;
    m_layers.append(layer);

    // 为每种层类型创建不同颜色和形状的图形项
    QColor color;
    if (layerType == "Input") color = Qt::cyan;
    else if (layerType == "Hidden") color = Qt::yellow;
    else if (layerType == "Output") color = Qt::magenta;
    else if (layerType == "Convolutional") color = Qt::green;
    else if (layerType == "Pooling") color = Qt::blue;
    else if (layerType == "LSTM") color = Qt::red;
    else if (layerType == "RNN") color = Qt::darkCyan;
    else if (layerType == "Dropout") color = Qt::gray;
    else if (layerType == "Dense") color = Qt::darkYellow;

    // 创建层的图形项并添加到场景
    QGraphicsRectItem* layerItem = new QGraphicsRectItem(0, 0, 100, 50);
    layerItem->setBrush(color);
    layerItem->setData(0, QVariant::fromValue(layer));
    m_builderScene->addItem(layerItem);

    // 设置属性面板
    QMap<QString, QString> params;
    params["LayerType"] = layerType;
    if(layerType =="Dense"){
        params["neurons"] = "10";
        params["activation"] = "ReLU";
    }
    m_propertyPanel->setParameters(params);
}

void CodeGeneratorWindow::on_propertiesPanel_parametersUpdated(const QMap<QString, QString>& params) {
    // 更新选中的层的参数
}

/*void CodeGeneratorWindow::on_generateCodeButton_clicked() {
    // 生成 PyTorch 代码
    QString code = CodeGenerator::generatePyTorchCode(m_layers);
    m_codeDisplay->setPlainText(code);
}*/
void CodeGeneratorWindow::on_generateCodeButton_clicked() {
    // 收集网络层信息
    QList<NeuralLayer> layers;
    // 假设从 m_layers 中收集用户选择的层信息
    layers = m_layers;

    // 生成 PyTorch 代码
    QString code = CodeGenerator::generatePyTorchCode(layers);
    m_codeDisplay->setPlainText(code);
}
void CodeGeneratorWindow::on_return_mainwindow_clicked()
{
    this->close();
    MainWindow::instance()->show();
}

