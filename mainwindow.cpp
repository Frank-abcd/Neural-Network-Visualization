#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "json_utils.h"
#include "backend.h"
#include "propertypanel.h"
#include <QIcon>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsRectItem>
#include <QJsonArray>
#include <QFile>
#include <QProgressBar>
#include <QDialog>
#include <QListWidget>

PropertyPanel* propertyPanel;

void MainWindow::setupIconButton(QPushButton* button, const QString& iconPath, int size) {
    button->setFixedSize(size, size);
    button->setIcon(QIcon(iconPath));
    button->setIconSize(QSize(size, size));
    button->setStyleSheet("background-color:transparent");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::generateJson);//使用示例

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

    QMenu* modeMenu = new QMenu(this);

    QAction* blockGenerateAction = new QAction("BlockGenerate 模式", this);

    modeMenu->addAction(blockGenerateAction);
    ui->mode->setMenu(modeMenu);

    connect(blockGenerateAction, &QAction::triggered, this, [=]() {
        currentMode = "BlockGenerate";
        showFloatingMessage("BlockGenerator");
    });

    scene = new QGraphicsScene(this);
}

void MainWindow::on_user_clicked()
{
    qDebug() << "user 按钮点击了";
}

void MainWindow::on_mode_clicked()
{
    qDebug() << "mode 按钮点击了";

}

void MainWindow::on_generate_code_clicked()
{
    qDebug() << "代码生成中";
}

void MainWindow::on_generate_image_clicked()
{
    if (currentMode=="unselected"){
        showFloatingMessage("Please select a mode.");
    }
    if (currentMode=="blockgenerate"){

    }
    if (currentMode==""){

    }
    qDebug() << "神经网络图像生成中";
}

void MainWindow::on_history_clicked()
{
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("历史记录");
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    QListWidget* list = new QListWidget(dialog);

    QFile file("history.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isArray()) {
            QJsonArray history = doc.array();
            for (const QJsonValue& val : history) {
                QString ts = val.toObject()["timestamp"].toString();
                QString mode = val.toObject()["mode"].toString();
                list->addItem(ts + " - " + mode);
            }
        }
    }

    layout->addWidget(list);

    // 加载按钮
    QPushButton* loadBtn = new QPushButton("加载选中记录");
    layout->addWidget(loadBtn);

    connect(loadBtn, &QPushButton::clicked, this, [=]() {
        int index = list->currentRow();
        if (index < 0) return;

        // 加载选中记录
        QFile file("history.json");
        if (file.open(QIODevice::ReadOnly)) {
            QJsonArray history = QJsonDocument::fromJson(file.readAll()).array();
            QJsonObject selected = history[index].toObject()["network"].toObject();
            QJsonArray layers = selected["layers"].toArray();

            loadNetworkFromJson(layers);  // 你写的载入函数
            showFloatingMessage("已载入历史记录！");
            dialog->accept();
        }
    });

    dialog->setLayout(layout);
    dialog->resize(400, 300);
    dialog->exec();
}

void MainWindow::on_start_new_clicked()
{

}

void MainWindow::on_previous_clicked()
{

}

void MainWindow::on_toolButton_clicked()
{

}

void MainWindow::on_turnback_clicked()
{

}

void MainWindow::on_save_clicked()
{
    QJsonArray layersArray = getCurrentNetworkAsJson();
    QJsonObject entry;
    entry["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    entry["mode"] = currentMode;
    entry["network"] = QJsonObject{ { "layers", layersArray } };

    QFile file("history.json");
    QJsonArray history;

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isArray()) history = doc.array();
        file.close();
    }

    history.append(entry);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonDocument newDoc(history);
        file.write(newDoc.toJson());
        file.close();
    }

    showSaveProgressBarMessage();
}


void MainWindow::handleJsonData(const QString &jsonStr) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        // 假设obj表示网络结构，从中提取层信息
        QJsonArray layersArray = obj["layers"].toArray();
        for (const QJsonValue &layerValue : layersArray) {
            QJsonObject layerObj = layerValue.toObject();
            NeuralLayer layer = NeuralLayer::fromJsonObject(layerObj);
            // 后续可对layer进行操作
        }
    }
}

void MainWindow::showFloatingMessage(const QString& text)
{
    QLabel* label = new QLabel(this);
    label->setText(text);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(
        "background-color: rgba(50, 50, 50, 180);"
        "color: white;"
        "font-size: 16px;"
        "padding: 10px;"
        "border-radius: 8px;"
        );
    label->adjustSize();

    int x = (width() - label->width()) / 2;
    int y = (height() - label->height()) / 10;
    label->move(x, y);
    label->show();

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(label);
    label->setGraphicsEffect(effect);
    effect->setOpacity(1.0);

    QPropertyAnimation* fadeOut = new QPropertyAnimation(effect, "opacity");
    fadeOut->setDuration(1000);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::OutQuad);

    QPropertyAnimation* moveUp = new QPropertyAnimation(label, "pos");
    moveUp->setDuration(1000);
    moveUp->setStartValue(QPoint(x, y));
    moveUp->setEndValue(QPoint(x, y - 50));
    moveUp->setEasingCurve(QEasingCurve::OutQuad);

    QParallelAnimationGroup* group = new QParallelAnimationGroup(label);
    group->addAnimation(fadeOut);
    group->addAnimation(moveUp);

    QTimer::singleShot(100, this, [group]() {
        group->start(QAbstractAnimation::DeleteWhenStopped);
    });

    connect(group, &QPropertyAnimation::finished, label, &QLabel::deleteLater);
}

QJsonArray MainWindow::getCurrentNetworkAsJson()
{
    QJsonArray layerArray;

    for (QGraphicsItem* item : scene->items()) {
        // 筛选我们添加的图层（跳过辅助线等）
        if (QGraphicsRectItem* rect = qgraphicsitem_cast<QGraphicsRectItem*>(item)) {
            QVariant data = rect->data(0);  // 通常第 0 位是层数据

            if (data.canConvert<QVariantMap>()) {
                QVariantMap layerData = data.toMap();

                QJsonObject layerObj;
                layerObj["layerType"] = layerData["layerType"].toString();

                // 保存位置信息
                QPointF pos = rect->pos();
                layerObj["x"] = int(pos.x());
                layerObj["y"] = int(pos.y());

                // 保存参数（如 neurons/activation）
                QJsonObject paramsObj;
                for (const QString& key : layerData.keys()) {
                    if (key != "layerType") {
                        paramsObj[key] = layerData[key].toString();
                    }
                }

                layerObj["params"] = paramsObj;
                layerArray.append(layerObj);
            }
        }
    }

    return layerArray;
}

void MainWindow::showSaveProgressBarMessage()
{
    QWidget* popup = new QWidget(this);
    popup->setStyleSheet("background-color: rgba(50, 50, 50, 180); border-radius: 10px;");
    popup->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* layout = new QVBoxLayout(popup);

    QLabel* label = new QLabel("✔ 已保存当前网络配置！");
    label->setStyleSheet("color: white; font-size: 16px;");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    QProgressBar* progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);
    progressBar->setFixedHeight(10);
    progressBar->setStyleSheet(R"(
        QProgressBar {
            background-color: rgba(255, 255, 255, 50);
            border: 1px solid white;
            border-radius: 5px;
        }
        QProgressBar::chunk {
            background-color: limegreen;
            border-radius: 5px;
        }
    )");
    layout->addWidget(progressBar);

    popup->setLayout(layout);
    popup->adjustSize();

    int x = (width() - popup->width()) / 2;
    int y = (height() - popup->height()) / 10;
    popup->move(x, y);
    popup->show();

    QTimer* timer = new QTimer(popup);
    int interval = 30;
    QObject::connect(timer, &QTimer::timeout, this, [=]() mutable {
        int val = progressBar->value();
        if (val >= 100) {
            timer->stop();
            popup->close();
        } else {
            progressBar->setValue(val + 2);
        }
    });
    timer->start(interval);
}

void MainWindow::loadNetworkFromJson(const QJsonArray& layers)
{
    showWarningMessage("调取该历史记录将会覆盖当前图层⚠️请先保存");
    scene->clear();

    for (const QJsonValue& value : layers) {
        QJsonObject obj = value.toObject();

        QString layerType = obj["layerType"].toString();
        int x = obj["x"].toInt();
        int y = obj["y"].toInt();

        QMap<QString, QString> params;
        QJsonObject paramObj = obj["params"].toObject();
        for (const QString& key : paramObj.keys()) {
            params[key] = paramObj[key].toString();
        }

        QGraphicsRectItem* item = new QGraphicsRectItem(0, 0, 100, 50);
        item->setPos(x, y);

        QColor color = Qt::gray;
        if (layerType == "Dense") color = Qt::yellow;
        else if (layerType == "Conv2D") color = Qt::blue;
        else if (layerType == "Dropout") color = Qt::darkGray;
        item->setBrush(color);

        QVariantMap data;
        data["layerType"] = layerType;
        for (const QString& key : params.keys()) {
            data[key] = params[key];
        }
        item->setData(0, data);

        scene->addItem(item);
    }

    showFloatingMessage("✅ 网络结构已成功恢复！");
}

void MainWindow::showWarningMessage(const QString& text)
{
    QLabel* label = new QLabel(this);
    label->setText("⚠ " + text);  // 加警告图标
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(
        "background-color: rgba(180, 30, 30, 200);"  // 深红半透明
        "color: white;"
        "font-size: 16px;"
        "padding: 10px;"
        "border: 2px solid #ffaaaa;"
        "border-radius: 8px;"
        );
    label->adjustSize();

    // 显示在中上位置
    int x = (width() - label->width()) / 2;
    int y = (height() - label->height()) / 10;
    label->move(x, y);
    label->show();

    // 添加淡出动画
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(label);
    label->setGraphicsEffect(effect);
    effect->setOpacity(1.0);

    QPropertyAnimation* fadeOut = new QPropertyAnimation(effect, "opacity");
    fadeOut->setDuration(1000);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::OutQuad);

    QTimer::singleShot(1500, this, [=]() {
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    });

    connect(fadeOut, &QPropertyAnimation::finished, label, &QLabel::deleteLater);
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
