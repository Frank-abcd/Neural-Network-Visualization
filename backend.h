#ifndef BACKEND_H
#define BACKEND_H

#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QList>
#include <QPointF>

class NeuralLayer
{
public:
    QString layerType;         // 层类型
    QString activationFunction; // 激活函数
    double leakyReluSlope;//leakyRelu 小于0部分斜率
    QPointF position;

    NeuralLayer();
    virtual ~NeuralLayer() = default;

    virtual QJsonObject toJsonObject() const;
    static NeuralLayer* fromJsonObject(const QJsonObject& obj);
    virtual QString toJson() const = 0;  // 序列化层配置
    virtual QString getType() const = 0; // 返回层类型标识

    void setPosition(QPointF pos) { position = pos; }
    QPointF getPosition() const { return position; }
};

// 全连接层
class DenseLayer : public NeuralLayer
{
public:
    int neurons;    // 神经元数量
    int inputSize;  // 输入维度

    DenseLayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
};

// 2D卷积层
class Conv2dLayer : public NeuralLayer
{
public:
    int inputChannels;  // 输入通道数
    int outputChannels; // 输出通道数
    int kernelSize;     // 卷积核大小
    int stride;         // 步长
    int padding;        // 填充

    Conv2dLayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
};

// 2D最大池化层
class MaxPool2dLayer : public NeuralLayer
{
public:
    int kernelSize; // 池化核大小
    int stride;     // 步长
    int padding;    // 填充

    MaxPool2dLayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
};

// 2D平均池化层
class AvgPool2dLayer : public NeuralLayer
{
public:
    int kernelSize; // 池化核大小
    int stride;     // 步长
    int padding;    // 填充

    AvgPool2dLayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
};

// LSTM层
class LSTMLayer : public NeuralLayer
{
public:
    int inputSize;       // 输入大小
    int hiddenSize;      // 隐藏层大小
    int numLayers;       // 层数
    bool batchFirst;     // 是否batch在前
    bool bidirectional;  // 是否双向
    bool returnSequence; // 是否返回序列

    LSTMLayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
};

// GRU层
class GRULayer : public NeuralLayer
{
public:
    int inputSize;       // 输入大小
    int hiddenSize;      // 隐藏层大小
    int numLayers;       // 层数
    bool batchFirst;     // 是否batch在前
    bool bidirectional;  // 是否双向
    bool returnSequence; // 是否返回序列

    GRULayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
};

// RNN层
class RNNLayer : public NeuralLayer
{
public:
    int inputSize;       // 输入大小
    int hiddenSize;      // 隐藏层大小
    int numLayers;       // 层数
    bool batchFirst;     // 是否batch在前
    bool bidirectional;  // 是否双向
    bool returnSequence; // 是否返回序列
    QString rnnActivation; // 激活函数(tanh/relu)

    RNNLayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
};

// Dropout层
class DropoutLayer : public NeuralLayer
{
public:
    double dropoutRate;  // dropout率

    DropoutLayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
};

// 批归一化层(1D)
class BatchNorm1dLayer : public NeuralLayer
{
public:
    int numFeatures; // 特征数量

    BatchNorm1dLayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
};

// 批归一化层(2D)
class BatchNorm2dLayer : public NeuralLayer
{
public:
    int numFeatures; // 特征数量

    BatchNorm2dLayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
};

// 展平层
class FlattenLayer : public NeuralLayer
{
public:
    FlattenLayer();
    QJsonObject toJsonObject() const override;
    bool fromJsonObject(const QJsonObject& obj);
    QString toJson() const override;
    QString getType() const override ;
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
