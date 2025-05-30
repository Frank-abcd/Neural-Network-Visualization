#include "mainwindow.h"
#include "backend.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "networkvisualizer.h"
#include "layerblockitem.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "CodeWings_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    // 构造一些测试数据
    QList<NeuralLayer> layers;
    NeuralLayer l1; l1.layerType = "input"; l1.inputSize = 18; l1.neurons = 18; l1.activationFunction = " ";
    NeuralLayer l2; l2.layerType = "hidden layer"; l2.inputSize = 18; l2.neurons = 10; l2.activationFunction = "linear";
    NeuralLayer l3; l3.layerType = "hidden layer"; l3.inputSize = 18; l3.neurons = 10; l3.activationFunction = "linear";
    NeuralLayer l4; l4.layerType = "output"; l4.inputSize = 18; l4.neurons = 1; l4.activationFunction = "softmax";
    layers.append(l1);
    layers.append(l2);
    layers.append(l3);
    layers.append(l4);

    NetworkVisualizer* visualizer = new NetworkVisualizer();
    visualizer->applyColorTheme("Ocean");
    visualizer->createNetwork(layers);
    visualizer->show();


    MainWindow w;
    MainWindow::setInstance(&w);
    w.show();
    return a.exec();

    /*
    visualizer->createblockNetwork(layers);
    visualizer->show();*/

    return a.exec();
}
