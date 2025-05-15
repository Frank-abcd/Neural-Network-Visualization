/*#include "backend.h"
#include "json_utils.h"
#include "codegenerator.h"
#include "programfragmentprocessor.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

QJsonObject NeuralLayer::toJsonObject() const {//将NeuralLayer对象转化为QJsonObject对象
    QJsonObject obj;
    obj["layerType"] = layerType;
    obj["neurons"] = neurons;
    obj["activationFunction"] = activationFunction;
    return obj;
}

NeuralLayer::NeuralLayer() {
    layerType = "";
    neurons = 0;
    activationFunction = "";
}

NeuralLayer NeuralLayer::fromJsonObject(const QJsonObject& obj) {//将QJsonObject对象转化为NeuralLayer对象
    NeuralLayer layer;
    if (!obj.contains("layerType") ||!obj.contains("neurons") ||!obj.contains("activationFunction")) {
        qDebug() << "Error form fromJsonObject: Missing required fields in JSON object when converting to NeuralLayer.";
        return layer;
    }
    layer.layerType = obj["layerType"].toString();
    layer.neurons = obj["neurons"].toInt();
    layer.activationFunction = obj["activationFunction"].toString();
    return layer;
}



// 接收网络结构数据接口
void receiveNetworkStructure(const QString& jsonStr) {
    QJsonArray layersArray = parseNetworkStructure(jsonStr);
    if (layersArray.isEmpty()) {
        qDebug() << "错误:解析的层数组为空";
        return;
    }

    QList<NeuralLayer> layers;
    for (const QJsonValue &layerValue : layersArray) {
        NeuralLayer layer = NeuralLayer::fromJsonObject(layerValue.toObject());
        layers.append(layer);
        qDebug() << "解析层:" << layer.layerType << layer.neurons;
    }

    // 生成 PyTorch 代码并返回
    QString generatedCode = CodeGenerator::generatePyTorchCode(layers);

    QJsonObject codeObj;
    codeObj["generatedCode"] = generatedCode;
    codeObj["language"] = "python";
    codeObj["framework"] = "pytorch";

    QJsonDocument doc(codeObj);
    returnGeneratedProgram(doc.toJson(QJsonDocument::Compact));
}

// 返回生成程序接口
void returnGeneratedProgram(const QString& programJsonStr) {
    // 实际项目中发送给前端
    qDebug() << "返回生成的程序:" << programJsonStr;
    //emit sendDataToFrontend(programJsonStr); // 假设这是一个信号
}

// 接收程序片段接口
void receiveProgramFragment(const QString& jsonStr) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isObject()) {
        qDebug() << "错误: JSON 不是对象";
        return;
    }

    QJsonObject fragmentObj = doc.object();
    QJsonObject result = ProgramFragmentProcessor::processFragment(fragmentObj);

    QJsonDocument resultDoc(result);
    QString resultJsonStr = resultDoc.toJson(QJsonDocument::Compact);

    qDebug() << "处理片段结果:" << resultJsonStr;
    //emit sendDataToFrontend(resultJsonStr); // 假设这是一个信号
}

// 返回网络结构接口
void returnNetworkStructure(const QString& networkStructureJsonStr) {
    // 实际项目中应通过网络发送给前端
    qDebug() << "返回网络结构:" << networkStructureJsonStr;
    //emit sendDataToFrontend(networkStructureJsonStr); // 假设这是一个信号
}*/
#include "backend.h"
#include "json_utils.h"
#include "codegenerator.h"
#include "programfragmentprocessor.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

// NeuralLayer 基类实现
NeuralLayer::NeuralLayer() : layerType(""), activationFunction("") ,leakyReluSlope(0.01){}

QJsonObject NeuralLayer::toJsonObject() const {
    QJsonObject obj;
    obj["layerType"] = layerType;
    obj["activationFunction"] = activationFunction;
    return obj;
}

NeuralLayer* NeuralLayer::fromJsonObject(const QJsonObject& obj) {//将QJsonObject对象转化为NeuralLayer对象,
    //修改:返回值类型由NeuralLayer变为NeuralLayer*,以支持多态

    if (!obj.contains("layerType")) {
        qDebug() << "Error: Missing layerType in JSON object";
        return nullptr;
    }

    QString type = obj["layerType"].toString();
    NeuralLayer* layer = nullptr;

    if (type == "Dense") {
        layer = new DenseLayer();
    } else if (type == "Conv2d") {
        layer = new Conv2dLayer();
    } else if (type == "MaxPool2d") {
        layer = new MaxPool2dLayer();
    } else if (type == "AvgPool2d") {
        layer = new AvgPool2dLayer();
    } else if (type == "LSTM") {
        layer = new LSTMLayer();
    } else if (type == "GRU") {
        layer = new GRULayer();
    } else if (type == "RNN") {
        layer = new RNNLayer();
    } else if (type == "Dropout") {
        layer = new DropoutLayer();
    } else if (type == "BatchNorm1d") {
        layer = new BatchNorm1dLayer();
    } else if (type == "BatchNorm2d") {
        layer = new BatchNorm2dLayer();
    } else if (type == "Flatten") {
        layer = new FlattenLayer();
    } else {
        qDebug() << "Unknown layer type:" << type;
        return nullptr;
    }

    // 公共属性
    if (obj.contains("activationFunction")) {
        layer->activationFunction = obj["activationFunction"].toString();
    }

    // 调用子类的fromJsonObject方法
    if (!layer->fromJsonObject(obj)) {
        delete layer;
        return nullptr;
    }

    return layer;
}

// DenseLayer 实现
DenseLayer::DenseLayer() : NeuralLayer(), neurons(0), inputSize(0) {
    layerType = "Dense";
}

QJsonObject DenseLayer::toJsonObject() const {
    QJsonObject obj = NeuralLayer::toJsonObject();
    obj["neurons"] = neurons;
    obj["inputSize"] = inputSize;
    return obj;
}

bool DenseLayer::fromJsonObject(const QJsonObject& obj) {
    if (!obj.contains("neurons") || !obj.contains("inputSize")) {
        qDebug() << "Error: Missing required fields for DenseLayer";
        return false;
    }
    neurons = obj["neurons"].toInt();
    inputSize = obj["inputSize"].toInt();
    return true;
}

// Conv2dLayer 实现
Conv2dLayer::Conv2dLayer() : NeuralLayer(), inputChannels(0), outputChannels(0),
    kernelSize(3), stride(1), padding(0) {
    layerType = "Conv2d";
}

QJsonObject Conv2dLayer::toJsonObject() const {
    QJsonObject obj = NeuralLayer::toJsonObject();
    obj["inputChannels"] = inputChannels;
    obj["outputChannels"] = outputChannels;
    obj["kernelSize"] = kernelSize;
    obj["stride"] = stride;
    obj["padding"] = padding;
    return obj;
}

bool Conv2dLayer::fromJsonObject(const QJsonObject& obj) {
    if (!obj.contains("inputChannels") || !obj.contains("outputChannels")) {
        qDebug() << "Error: Missing required fields for Conv2dLayer";
        return false;
    }
    inputChannels = obj["inputChannels"].toInt();
    outputChannels = obj["outputChannels"].toInt();
    kernelSize = obj.value("kernelSize").toInt(3);
    stride = obj.value("stride").toInt(1);
    padding = obj.value("padding").toInt(0);
    return true;
}

// MaxPool2dLayer 实现
MaxPool2dLayer::MaxPool2dLayer() : NeuralLayer(), kernelSize(2), stride(2), padding(0) {
    layerType = "MaxPool2d";
}

QJsonObject MaxPool2dLayer::toJsonObject() const {
    QJsonObject obj = NeuralLayer::toJsonObject();
    obj["kernelSize"] = kernelSize;
    obj["stride"] = stride;
    obj["padding"] = padding;
    return obj;
}

bool MaxPool2dLayer::fromJsonObject(const QJsonObject& obj) {
    kernelSize = obj.value("kernelSize").toInt(2);
    stride = obj.value("stride").toInt(2);
    padding = obj.value("padding").toInt(0);
    return true;
}

// AvgPool2dLayer 实现
AvgPool2dLayer::AvgPool2dLayer() : NeuralLayer(), kernelSize(2), stride(2), padding(0) {
    layerType = "AvgPool2d";
}

QJsonObject AvgPool2dLayer::toJsonObject() const {
    QJsonObject obj = NeuralLayer::toJsonObject();
    obj["kernelSize"] = kernelSize;
    obj["stride"] = stride;
    obj["padding"] = padding;
    return obj;
}

bool AvgPool2dLayer::fromJsonObject(const QJsonObject& obj) {
    kernelSize = obj.value("kernelSize").toInt(2);
    stride = obj.value("stride").toInt(2);
    padding = obj.value("padding").toInt(0);
    return true;
}

// LSTMLayer 实现
LSTMLayer::LSTMLayer() : NeuralLayer(), inputSize(0), hiddenSize(0), numLayers(1),
    batchFirst(false), bidirectional(false), returnSequence(false) {
    layerType = "LSTM";
}

QJsonObject LSTMLayer::toJsonObject() const {
    QJsonObject obj = NeuralLayer::toJsonObject();
    obj["inputSize"] = inputSize;
    obj["hiddenSize"] = hiddenSize;
    obj["numLayers"] = numLayers;
    obj["batchFirst"] = batchFirst;
    obj["bidirectional"] = bidirectional;
    obj["returnSequence"] = returnSequence;
    return obj;
}

bool LSTMLayer::fromJsonObject(const QJsonObject& obj) {
    if (!obj.contains("inputSize") || !obj.contains("hiddenSize")) {
        qDebug() << "Error: Missing required fields for LSTMLayer";
        return false;
    }
    inputSize = obj["inputSize"].toInt();
    hiddenSize = obj["hiddenSize"].toInt();
    numLayers = obj.value("numLayers").toInt(1);
    batchFirst = obj.value("batchFirst").toBool(false);
    bidirectional = obj.value("bidirectional").toBool(false);
    returnSequence = obj.value("returnSequence").toBool(false);
    return true;
}

// GRULayer 实现
GRULayer::GRULayer() : NeuralLayer(), inputSize(0), hiddenSize(0), numLayers(1),
    batchFirst(false), bidirectional(false), returnSequence(false) {
    layerType = "GRU";
}

QJsonObject GRULayer::toJsonObject() const {
    QJsonObject obj = NeuralLayer::toJsonObject();
    obj["inputSize"] = inputSize;
    obj["hiddenSize"] = hiddenSize;
    obj["numLayers"] = numLayers;
    obj["batchFirst"] = batchFirst;
    obj["bidirectional"] = bidirectional;
    obj["returnSequence"] = returnSequence;
    return obj;
}

bool GRULayer::fromJsonObject(const QJsonObject& obj) {
    if (!obj.contains("inputSize") || !obj.contains("hiddenSize")) {
        qDebug() << "Error: Missing required fields for GRULayer";
        return false;
    }
    inputSize = obj["inputSize"].toInt();
    hiddenSize = obj["hiddenSize"].toInt();
    numLayers = obj.value("numLayers").toInt(1);
    batchFirst = obj.value("batchFirst").toBool(false);
    bidirectional = obj.value("bidirectional").toBool(false);
    returnSequence = obj.value("returnSequence").toBool(false);
    return true;
}

// RNNLayer 实现
RNNLayer::RNNLayer() : NeuralLayer(), inputSize(0), hiddenSize(0), numLayers(1),
    batchFirst(false), bidirectional(false), returnSequence(false),
    rnnActivation("tanh") {
    layerType = "RNN";
}

QJsonObject RNNLayer::toJsonObject() const {
    QJsonObject obj = NeuralLayer::toJsonObject();
    obj["inputSize"] = inputSize;
    obj["hiddenSize"] = hiddenSize;
    obj["numLayers"] = numLayers;
    obj["batchFirst"] = batchFirst;
    obj["bidirectional"] = bidirectional;
    obj["returnSequence"] = returnSequence;
    obj["rnnActivation"] = rnnActivation;
    return obj;
}

bool RNNLayer::fromJsonObject(const QJsonObject& obj) {
    if (!obj.contains("inputSize") || !obj.contains("hiddenSize")) {
        qDebug() << "Error: Missing required fields for RNNLayer";
        return false;
    }
    inputSize = obj["inputSize"].toInt();
    hiddenSize = obj["hiddenSize"].toInt();
    numLayers = obj.value("numLayers").toInt(1);
    batchFirst = obj.value("batchFirst").toBool(false);
    bidirectional = obj.value("bidirectional").toBool(false);
    returnSequence = obj.value("returnSequence").toBool(false);
    rnnActivation = obj.value("rnnActivation").toString("tanh");
    return true;
}

// DropoutLayer 实现
DropoutLayer::DropoutLayer() : NeuralLayer(), dropoutRate(0.5) {
    layerType = "Dropout";
}

QJsonObject DropoutLayer::toJsonObject() const {
    QJsonObject obj = NeuralLayer::toJsonObject();
    obj["dropoutRate"] = dropoutRate;
    return obj;
}

bool DropoutLayer::fromJsonObject(const QJsonObject& obj) {
    dropoutRate = obj.value("dropoutRate").toDouble(0.5);
    return true;
}

// BatchNorm1dLayer 实现
BatchNorm1dLayer::BatchNorm1dLayer() : NeuralLayer(), numFeatures(0) {
    layerType = "BatchNorm1d";
}

QJsonObject BatchNorm1dLayer::toJsonObject() const {
    QJsonObject obj = NeuralLayer::toJsonObject();
    obj["numFeatures"] = numFeatures;
    return obj;
}

bool BatchNorm1dLayer::fromJsonObject(const QJsonObject& obj) {
    if (!obj.contains("numFeatures")) {
        qDebug() << "Error: Missing numFeatures for BatchNorm1dLayer";
        return false;
    }
    numFeatures = obj["numFeatures"].toInt();
    return true;
}

// BatchNorm2dLayer 实现
BatchNorm2dLayer::BatchNorm2dLayer() : NeuralLayer(), numFeatures(0) {
    layerType = "BatchNorm2d";
}

QJsonObject BatchNorm2dLayer::toJsonObject() const {
    QJsonObject obj = NeuralLayer::toJsonObject();
    obj["numFeatures"] = numFeatures;
    return obj;
}

bool BatchNorm2dLayer::fromJsonObject(const QJsonObject& obj) {
    if (!obj.contains("numFeatures")) {
        qDebug() << "Error: Missing numFeatures for BatchNorm2dLayer";
        return false;
    }
    numFeatures = obj["numFeatures"].toInt();
    return true;
}

// FlattenLayer 实现
FlattenLayer::FlattenLayer() {
    layerType = "Flatten";
}

QJsonObject FlattenLayer::toJsonObject() const {
    return NeuralLayer::toJsonObject();
}

bool FlattenLayer::fromJsonObject(const QJsonObject&) {
    return true;
}


// 接口函数实现
void receiveNetworkStructure(const QString& jsonStr) {
    QJsonArray layersArray = parseNetworkStructure(jsonStr);
    if (layersArray.isEmpty()) {
        qDebug() << "错误:解析的层数组为空";
        return;
    }

    QList<NeuralLayer*> layers;
    for (const QJsonValue &layerValue : layersArray) {
        NeuralLayer* layer = NeuralLayer::fromJsonObject(layerValue.toObject());
        if (layer) {
            layers.append(layer);
            qDebug() << "解析层:" << layer->layerType;
        }
    }

    // 生成 PyTorch 代码并返回
    QString generatedCode = CodeGenerator::generatePyTorchCode(layers);

    QJsonObject codeObj;
    codeObj["generatedCode"] = generatedCode;
    codeObj["language"] = "python";
    codeObj["framework"] = "pytorch";

    QJsonDocument doc(codeObj);
    returnGeneratedProgram(doc.toJson(QJsonDocument::Compact));

    // 释放内存
    qDeleteAll(layers);
}

void returnGeneratedProgram(const QString& programJsonStr) {
    qDebug() << "返回生成的程序:" << programJsonStr;
}

void receiveProgramFragment(const QString& jsonStr) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isObject()) {
        qDebug() << "错误: JSON 不是对象";
        return;
    }

    QJsonObject fragmentObj = doc.object();
    QJsonObject result = ProgramFragmentProcessor::processFragment(fragmentObj);

    QJsonDocument resultDoc(result);
    QString resultJsonStr = resultDoc.toJson(QJsonDocument::Compact);

    qDebug() << "处理片段结果:" << resultJsonStr;
}

void returnNetworkStructure(const QString& networkStructureJsonStr) {
    qDebug() << "返回网络结构:" << networkStructureJsonStr;
}
