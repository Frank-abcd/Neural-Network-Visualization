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

NeuralLayer* NeuralLayer::fromJsonObject(const QJsonObject& obj) {
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

    if (!layer) {
        return nullptr;
    }
    if (!layer->fromJsonObject(obj)) {
        delete layer; // 防止内存泄漏
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

QString DenseLayer::toJson() const  {
    return QString("{\"type\":\"dense\",\"neurons\":%1,\"activation\":\"%2\"}")
    .arg(neurons).arg(activationFunction);
}
QString DenseLayer::getType() const  { return "dense"; }

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

QString Conv2dLayer::toJson() const {
    return QString("{\"type\":\"conv2d\",\"input_channels\":%1,\"output_channels\":%2,\"kernel_size\":%3,\"stride\":%4,\"padding\":%5,\"activation\":\"%6\",\"leaky_relu_slope\":%7}")
    .arg(inputChannels).arg(outputChannels).arg(kernelSize)
        .arg(stride).arg(padding).arg(activationFunction).arg(leakyReluSlope);
}

QString Conv2dLayer::getType() const { return "conv2d"; }

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

QString MaxPool2dLayer::toJson() const {
    return QString("{\"type\":\"maxpool2d\",\"kernel_size\":%1,\"stride\":%2,\"padding\":%3}")
    .arg(kernelSize).arg(stride).arg(padding);
}

QString MaxPool2dLayer::getType() const { return "maxpool2d"; }

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

QString AvgPool2dLayer::toJson() const {
    return QString("{\"type\":\"avgpool2d\",\"kernel_size\":%1,\"stride\":%2,\"padding\":%3}")
    .arg(kernelSize).arg(stride).arg(padding);
}

QString AvgPool2dLayer::getType() const { return "avgpool2d"; }

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

QString LSTMLayer::toJson() const {
    return QString("{\"type\":\"lstm\",\"input_size\":%1,\"hidden_size\":%2,\"num_layers\":%3,\"batch_first\":%4,\"bidirectional\":%5,\"return_sequence\":%6}")
    .arg(inputSize).arg(hiddenSize).arg(numLayers)
        .arg(batchFirst ? "true" : "false")
        .arg(bidirectional ? "true" : "false")
        .arg(returnSequence ? "true" : "false");
}

QString LSTMLayer::getType() const { return "lstm"; }

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

QString GRULayer::toJson() const {
    return QString("{\"type\":\"gru\",\"input_size\":%1,\"hidden_size\":%2,\"num_layers\":%3,\"batch_first\":%4,\"bidirectional\":%5,\"return_sequence\":%6}")
    .arg(inputSize).arg(hiddenSize).arg(numLayers)
        .arg(batchFirst ? "true" : "false")
        .arg(bidirectional ? "true" : "false")
        .arg(returnSequence ? "true" : "false");
}

QString GRULayer::getType() const { return "gru"; }

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

QString RNNLayer::toJson() const {
    return QString("{\"type\":\"rnn\",\"input_size\":%1,\"hidden_size\":%2,\"num_layers\":%3,\"batch_first\":%4,\"bidirectional\":%5,\"return_sequence\":%6,\"rnn_activation\":\"%7\"}")
    .arg(inputSize).arg(hiddenSize).arg(numLayers)
        .arg(batchFirst ? "true" : "false")
        .arg(bidirectional ? "true" : "false")
        .arg(returnSequence ? "true" : "false")
        .arg(rnnActivation);
}

QString RNNLayer::getType() const { return "rnn"; }

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

QString DropoutLayer::toJson() const {
    return QString("{\"type\":\"dropout\",\"dropout_rate\":%1}").arg(dropoutRate);
}

QString DropoutLayer::getType() const { return "dropout"; }

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

QString BatchNorm1dLayer::toJson() const {
    return QString("{\"type\":\"batchnorm1d\",\"num_features\":%1}").arg(numFeatures);
}

QString BatchNorm1dLayer::getType() const { return "batchnorm1d"; }

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

QString BatchNorm2dLayer::toJson() const {
    return QString("{\"type\":\"batchnorm2d\",\"num_features\":%1}").arg(numFeatures);
}

QString BatchNorm2dLayer::getType() const { return "batchnorm2d"; }

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

QString FlattenLayer::toJson() const {
    return QString("{\"type\":\"flatten\"}");
}

QString FlattenLayer::getType() const { return "flatten"; }


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
