#ifndef BACKEND_H
#define BACKEND_H

#include <QJsonObject>
#include <QJsonArray>
#include <QString>

class NeuralLayer
{
public:

    QString layerType;//层定义
    int neurons = 1000;//神经元数量
    int inputSize = 128;//输入维度
    QString activationFunction = "relu";//激活函数
    NeuralLayer();
    QJsonObject toJsonObject()const;//将NeuralLayer对象转化为QJsonObject，以后可拼接为QJsonArray
    static NeuralLayer fromJsonObject(const QJsonObject& obj);// 从QJsonObject构造NeuralLayer的静态函数声明
    //obj 应该包含"layerType" "neurons" "activationFunction"

    // 卷积层特有参数
    int filters = 0;
    int kernelSize = 0;

    // 池化层特有参数
    int poolingSize = 0;

    // LSTM 或 RNN 特有参数
    int units = 0;

    // Dropout 层特有参数
    float dropoutRate = 0.0f;
};


#endif // BACKEND_H
