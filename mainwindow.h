#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <backend.h>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QLinearGradient>
#include <QPen>
#include <QPlainTextEdit>
#include <QGraphicsView>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class NetworkGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit NetworkGraphicsView(QWidget *parent = nullptr);

signals:
    void layerDropped(const QString& layerType, const QPointF& position);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void handleJsonData(const QString &jsonStr);
    QGraphicsScene* networkScene; // 添加场景成员变量
    QPlainTextEdit* codePreview;
    void startDrag(const QString& layerType);
    void onLayerDropped(const QString& layerType, const QPointF& position);

private:
    NetworkGraphicsView* networkView;  // 添加自定义视图成员变量
    QList<QGraphicsItem*> connectionItems; // 添加连接线列表
    void updateConnections();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_generate_code_clicked();

private:
    Ui::MainWindow *ui;
    void setupIconButton(QPushButton* button, const QString& iconPath, int size = 40);
    QList<NeuralLayer*> selectedLayers;// 添加选中的图层列表
    QMap<QPushButton*, QString> layerButtonMap; // 按钮到层类型的映射

    QGraphicsItem* createLayerGraphicsItem(NeuralLayer* layer);
    void addConnectionPoint(QGraphicsItem* parent, const QPointF& pos, bool isInput);
};

#endif // MAINWINDOW_H
