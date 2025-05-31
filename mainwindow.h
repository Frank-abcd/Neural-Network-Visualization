#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QJsonArray>
#include <QGraphicsScene>
#include "codegeneratorwindow.h"
#include "networkvisualizer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void handleJsonData(const QString &jsonStr);
    //void generateJson();//使用示例
    /*
    static MainWindow* instance() {
        static MainWindow* instance = new MainWindow();
        return instance;
    }*/
    static MainWindow* instance();
    static void setInstance(MainWindow* window);//
    QString currentMode = "unselected";
    void showFloatingMessage(const QString& text);
    QJsonArray getCurrentNetworkAsJson();
    void showSaveProgressBarMessage();
    void loadNetworkFromJson(const QJsonArray& layers);
    void showWarningMessage(const QString& text);
    void visualizeNetwork(const QJsonArray& layers);
    void onHistoryRecordClicked(const QString& recordKey);
    bool currentNetworkSaved;
    QJsonArray loadHistoryByKey(const QString& key);
    void showNetworkVisualization(const QJsonArray& layers);

private:
    Ui::MainWindow *ui;
    static MainWindow* s_instance;  // 添加这行
    void setupIconButton(QPushButton* button, const QString& iconPath, int size = 40);
    void setBackground(const QString& background);
    CodeGeneratorWindow* codegeneratorwindow;
    QGraphicsScene* scene;
    QJsonArray m_cachedNetworkJson;
    CodeGeneratorWindow* codeWin = nullptr;
    NetworkVisualizer* visualizer = nullptr;

private slots:
    void on_user_clicked();
    void on_mode_clicked();
    void on_generate_code_clicked();
    void on_generate_image_clicked();

    void on_toolButton_clicked();

    void on_history_clicked();
    void on_start_new_clicked();
    void on_previous_clicked();
    void on_turnback_clicked();
    void on_save_clicked();

};
#endif // MAINWINDOW_H
