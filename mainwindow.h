#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

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

private:
    Ui::MainWindow *ui;
    void setupIconButton(QPushButton* button, const QString& iconPath, int size = 40);

private slots:
    void on_user_clicked();
    void on_mode_clicked();
    void on_generate_code_clicked();
    void on_generate_image_clicked();

    void on_toolButton_clicked();
};
#endif // MAINWINDOW_H
