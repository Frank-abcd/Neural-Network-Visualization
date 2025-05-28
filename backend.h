#ifndef BACKEND_H
#define BACKEND_H

#include <QJsonObject>
#include <QJsonArray>
#include <QString>

class NeuralLayer
{
public:

    QString layerType;//层定义
    int neurons;//神经元数量
    int inputSize;//输入维度
    QString activationFunction;//激活函数
    NeuralLayer();
    QJsonObject toJsonObject()const;//将NeuralLayer对象转化为QJsonObject，以后可拼接为QJsonArray
    static NeuralLayer fromJsonObject(const QJsonObject& obj);// 从QJsonObject构造NeuralLayer的静态函数声明
    //obj 应该包含"layerType" "neurons" "activationFunction"
};


// 接收网络结构数据接口
void receiveNetworkStructure(const QString& jsonStr);

// 返回生成程序接口
void returnGeneratedProgram(const QString& programJsonStr);

// 接收程序片段接口
void receiveProgramFragment(const QString& jsonStr);

// 返回网络结构接口
void returnNetworkStructure(const QString& networkStructureJsonStr);

#endif // BACKEND_H
