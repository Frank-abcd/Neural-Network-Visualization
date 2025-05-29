#ifndef CODEGENERATORWINDOW_H
#define CODEGENERATORWINDOW_H

#include <QDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTextEdit>
#include <QListWidget>
#include <QStackedWidget>
#include "networkvisualizer.h"
#include "propertypanel.h"

namespace Ui {
class CodeGeneratorWindow;
}

class CodeGeneratorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CodeGeneratorWindow(QWidget *parent = nullptr);
    ~CodeGeneratorWindow();

private slots:
    void on_return_mainwindow_clicked();
    void on_generateCodeButton_clicked();
    void on_layersList_itemClicked(QListWidgetItem* item);
    void on_propertiesPanel_parametersUpdated(const QMap<QString, QString>& params);


private:
    Ui::CodeGeneratorWindow *ui;
    NetworkVisualizer* m_networkVisualizer;
    QGraphicsView* m_builderView;
    QGraphicsScene* m_builderScene;
    QTextEdit* m_codeDisplay;
    PropertyPanel* m_propertyPanel;
    QList<NeuralLayer> m_layers;

};

#endif // CODEGENERATORWINDOW_H
