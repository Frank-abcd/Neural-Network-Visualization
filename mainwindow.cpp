#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "json_utils.h"
#include "backend.h"
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::generateJson);//使用示例
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
            // 后续可对layer进行操作
            if(layer) delete layer;
        }
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
/*void MainWindow::generateJson()//使用示例
{
    NeuralLayer layer1;
    layer1.layerType = "Input";
    layer1.neurons = 10;
    layer1.activationFunction = "None";

    NeuralLayer layer2;
    layer2.layerType = "Hidden";
    layer2.neurons = 20;
    layer2.activationFunction = "ReLU";

    NeuralLayer layer3;
    layer2.layerType = "Output";
    layer2.neurons = 5;
    layer2.activationFunction = "ReLU";

    QJsonArray layersArray;
    layersArray.append(layer1.toJsonObject());
    layersArray.append(layer2.toJsonObject());
    layersArray.append(layer3.toJsonObject());

    QJsonDocument doc(layersArray);
    QString jsonStr = QString::fromUtf8(doc.toJson());
    qDebug() << jsonStr;
}*/

