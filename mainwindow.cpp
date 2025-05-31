#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "json_utils.h"
#include "backend.h"
#include "propertypanel.h"
#include <QIcon>
#include <QPushButton>
#include <QJsonDocument>
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
#include <QMessageBox>

PropertyPanel* propertyPanel;

void MainWindow::setupIconButton(QPushButton* button, const QString& iconPath, int size) {
    button->setFixedSize(3*size, size);
    button->setIcon(QIcon(iconPath));
    button->setIconSize(QSize(size, size));
    button->setStyleSheet("background-color:transparent");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    ui->user->setToolTip("CodeWings用户使用介绍");
    ui->mode->setToolTip("切换显示模式");
    ui->generate_code->setToolTip("生成 PyTorch 或 TensorFlow 代码");
    ui->generate_image->setToolTip("生成网络结构图像");
    ui->history->setToolTip("查看已保存的历史");
    ui->start_new->setToolTip("开始新的神经网络");
    ui->previous->setToolTip("返回上一步");
    ui->turnback->setToolTip("前进到下一步");
    ui->save->setToolTip("保存当前神经网络结构");

    codegeneratorwindow = new CodeGeneratorWindow(this);
    connect(ui->generate_code, &QPushButton::clicked, this, &MainWindow::on_generate_code_clicked);
    QMenu* modeMenu = new QMenu(this);

    QAction* blockGenerateAction = new QAction("BlockGenerate 模式", this);

    modeMenu->addAction(blockGenerateAction);
    ui->mode->setMenu(modeMenu);

    connect(blockGenerateAction, &QAction::triggered, this, [=]() {
        currentMode = "BlockGenerate";
        showFloatingMessage("BlockGenerator");
    });

    scene = new QGraphicsScene(this);

    currentNetworkSaved=0;
}

void MainWindow::on_user_clicked()
{
    qDebug() << "user 按钮点击了";
    // 在这里实现你希望的功能逻辑
}

void MainWindow::on_mode_clicked()
{
    qDebug() << "mode 按钮点击了";

}

void MainWindow::on_generate_code_clicked()
{
    if (!codeWin) {
        codeWin = new CodeGeneratorWindow(this);
    }

    this->hide();              // 隐藏主界面
    codeWin->show();           // 显示弹窗
}

void MainWindow::on_generate_image_clicked()
{
    if (!codeWin) {
        showWarningMessage("尚未创建网络结构！");
        return;
    }

    QJsonArray structure = codeWin->getNetworkAsJson();
    QList<NeuralLayer> layers;
    for (const QJsonValue& val : structure) {
        if (val.isObject()) {
            layers.append(NeuralLayer::fromJsonObject(val.toObject()));
        }
    }

    if (structure.isEmpty()) {
        showWarningMessage("网络结构为空，无法生成图像！");
        return;
    }

    // 调用你已有的神经网络图像生成逻辑（比如显示在主界面某个区域）
    NetworkVisualizer* visualizer = new NetworkVisualizer();
    visualizer->applyColorTheme("Ocean");
    visualizer->createNetwork(layers);
    visualizer->show();// 你来实现这个函数，基于 structure 展示图像
    ui->scrollAreavisualizer->setWidget(visualizer);
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

void MainWindow::onHistoryRecordClicked(const QString& recordKey)
{
    // 1. 如果当前结构未保存，弹出确认对话框
    if (!currentNetworkSaved) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "未保存更改",
            "当前网络结构尚未保存，是否继续查看所选历史记录？\n继续将丢弃当前修改。",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            return;  // 返回当前记录，不做任何操作
        }
    }

    // 2. 用户确认继续后，加载选中记录
    QJsonArray data = loadHistoryByKey(recordKey);
    showNetworkVisualization(data);
    showFloatingMessage("✅ 已加载历史记录：" + recordKey);
}

QJsonArray MainWindow::loadHistoryByKey(const QString& key)
{
    QFile file("history.json");
    if (!file.open(QIODevice::ReadOnly)) return {};
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    return doc.object().value(key).toArray();
}

void MainWindow::showNetworkVisualization(const QJsonArray& layers)
{
    // 1. 清空预览区域
    QLayout* layout = ui->previewArea->layout();
    if (!layout) {
        layout = new QVBoxLayout(ui->previewArea);
        ui->previewArea->setLayout(layout);
    }
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // 2. 创建 NetworkVisualizer 实例（或重用）
    if (!visualizer) {
        visualizer = new NetworkVisualizer(this);  // 你已定义的绘图类
    }

    // visualizer->createNetwork(layers);  // 设置图数据并触发更新
    layout->addWidget(visualizer);
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

    currentNetworkSaved=1;
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

void MainWindow::visualizeNetwork(const QJsonArray& layers)
{
    QGraphicsScene* scene = new QGraphicsScene(this);

    int x = 50;
    int y = 50;
    int spacing = 150;

    for (const QJsonValue& val : layers) {
        QJsonObject obj = val.toObject();
        QString type = obj["layerType"].toString();

        // 1. 图层框（灰框）
        QGraphicsRectItem* item = new QGraphicsRectItem(0, 0, 120, 60);
        item->setPos(x, y);

        QColor color = Qt::lightGray;
        if (type == "Input") color = Qt::cyan;
        else if (type == "Dense") color = Qt::yellow;
        else if (type == "Dropout") color = Qt::gray;
        else if (type == "LSTM") color = Qt::green;
        else if (type == "RNN") color = Qt::blue;

        item->setBrush(color);
        scene->addItem(item);

        // 2. 类型文字
        QGraphicsTextItem* text = new QGraphicsTextItem(type);
        text->setPos(x + 10, y + 20);
        scene->addItem(text);

        // 3. 准备下一层位置
        y += spacing;
    }

    // 展示到画布
    QGraphicsView* view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumHeight(400);

    // 自适应缩放（可选）
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // 清空展示区并添加
    QLayout* layout = ui->previewArea->layout();
    if (!layout) {
        layout = new QVBoxLayout(ui->previewArea);
        ui->previewArea->setLayout(layout);
    } else {
        QLayoutItem* item;
        while ((item = layout->takeAt(0))) {
            delete item->widget();
            delete item;
        }
    }

    layout->addWidget(view);
}


MainWindow::~MainWindow()
{
    delete ui;
}
// 静态成员定义
MainWindow* MainWindow::s_instance = nullptr;

MainWindow* MainWindow::instance() {
    return s_instance;
}

void MainWindow::setInstance(MainWindow* window) {
    s_instance = window;
}
