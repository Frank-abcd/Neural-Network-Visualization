#include "programfragmentprocessor.h"


ProgramFragmentProcessor::ProgramFragmentProcessor() {}

QJsonObject ProgramFragmentProcessor::processFragment(const QJsonObject& fragmentObj) {
    QString language = fragmentObj["language"].toString();
    QString code = fragmentObj["code"].toString();
    QString action = fragmentObj["action"].toString();

    QJsonObject result;
    result["status"] = "success";
    result["timestamp"] = QDateTime::currentDateTime().toString();

    if (action == "validate") {
        result["validationResult"] = validateCode(code);
    }
    else if (action == "extract-structure") {
        if (language == "python" && code.contains("torch.nn")) {
            result["networkStructure"] = extractPyTorchStructure(code);
        } else {
            result["error"] = "仅支持解析 PyTorch 代码";
        }
    }
    else {
        result["error"] = "不支持的操作: " + action;
    }

    return result;
}


// 从 PyTorch 代码中提取网络结构
QJsonArray ProgramFragmentProcessor::extractPyTorchStructure(const QString& code) {
    QJsonArray layersArray;

    // 提取 nn.Linear 层
    QRegularExpression linearPattern("nn\\.Linear\\s*\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\)");
    QRegularExpressionMatchIterator linearMatches = linearPattern.globalMatch(code);

    while (linearMatches.hasNext()) {
        QRegularExpressionMatch match = linearMatches.next();
        int inputSize = match.captured(1).toInt();
        int outputSize = match.captured(2).toInt();

        QJsonObject layerObj;
        layerObj["layerType"] = "Dense";
        layerObj["inputSize"] = inputSize;
        layerObj["neurons"] = outputSize;

        // 提取激活函数
        QString activation = extractActivationFunction(code, match.capturedStart());
        layerObj["activationFunction"] = activation;

        layersArray.append(layerObj);
    }

    // 提取 Conv2d 层
    QRegularExpression convPattern("nn\\.Conv2d\\s*\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*kernel_size\\s*=\\s*(\\d+)");
    QRegularExpressionMatchIterator convMatches = convPattern.globalMatch(code);

    while (convMatches.hasNext()) {
        QRegularExpressionMatch match = convMatches.next();
        int inChannels = match.captured(1).toInt();
        int outChannels = match.captured(2).toInt();
        int kernelSize = match.captured(3).toInt();

        QJsonObject layerObj;
        layerObj["layerType"] = "Conv2d";
        layerObj["inputSize"] = inChannels;  // 简化处理
        layerObj["neurons"] = outChannels;  // 简化处理
        layerObj["kernelSize"] = kernelSize;

        // 提取激活函数
        QString activation = extractActivationFunction(code, match.capturedStart());
        layerObj["activationFunction"] = activation;

        layersArray.append(layerObj);
    }

    return layersArray;
}

// 从代码中提取激活函数
QString ProgramFragmentProcessor::extractActivationFunction(const QString& code, int startPos) {
    QString snippet = code.mid(startPos, 200);

    if (snippet.contains("nn.ReLU") || snippet.contains("F.relu")) return "relu";
    if (snippet.contains("nn.Sigmoid") || snippet.contains("F.sigmoid")) return "sigmoid";
    if (snippet.contains("nn.Softmax") || snippet.contains("F.softmax")) return "softmax";
    if (snippet.contains("nn.Tanh") || snippet.contains("F.tanh")) return "tanh";

    return "";
}

// 简单的代码验证
QJsonObject ProgramFragmentProcessor::validateCode(const QString& code) {
    QJsonObject validationResult;
    validationResult["valid"] = true;
    validationResult["errors"] = QJsonArray();

    // 检查常见语法错误,未实现

    return validationResult;
}
