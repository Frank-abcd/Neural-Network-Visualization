#include "codegenerator.h"
#include <QList>
#include <QJsonDocument>

QString CodeGenerator::generatePyTorchCode(const QList<NeuralLayer*>& layers) {
    QString code = "# PyTorch 神经网络自动生成代码\n";
    code += "import torch\n";
    code += "import torch.nn as nn\n\n";
    code += "class Net(nn.Module):\n";
    code += "    def __init__(self):\n";
    code += "        super(Net, self).__init__()\n";

    // 生成层定义
    for (int i = 0; i < layers.size() ; ++i) {
        const NeuralLayer* layer = layers[layers.size()-1-i];
        if (layer->layerType == "Dense" || layer->layerType == "Input" ||
            layer->layerType == "Output" || layer->layerType == "Hidden") {
            code += QString("        self.fc%1 = nn.Linear(%2, %3)\n")
            .arg(i + 1)
                .arg(layer->inputSize)
                .arg(layer->neurons);
        } else if (layer->layerType == "Convolutional") {
            code += QString("        self.conv%1 = nn.Conv2d(%2, %3, kernel_size=%4)\n")
                        .arg(i + 1)
                        .arg(layer->inputSize)  // 输入通道数
                        .arg(layer->filters)    // 输出通道数
                        .arg(layer->kernelSize);
        } else if (layer->layerType == "MaxPooling") {
            code += QString("        self.pool%1 = nn.MaxPool2d(kernel_size=%2)\n")
                        .arg(i + 1)
                        .arg(layer->poolingSize);
        } else if (layer->layerType == "AveragePooling") {
            code += QString("        self.pool%1 = nn.AvgPool2d(kernel_size=%2)\n")
                        .arg(i + 1)
                        .arg(layer->poolingSize);
        } else if (layer->layerType == "LSTM") {
            code += QString("        self.lstm%1 = nn.LSTM(%2, %3)\n")
                        .arg(i + 1)
                        .arg(layer->inputSize)
                        .arg(layer->units);
        } else if (layer->layerType == "RNN") {
            code += QString("        self.rnn%1 = nn.RNN(%2, %3)\n")
                        .arg(i + 1)
                        .arg(layer->inputSize)
                        .arg(layer->units);
        }
        else if (layer->layerType == "GRU") {
            code += QString("        self.gru%1 = nn.GRU(%2, %3)\n")
                        .arg(i + 1)
                        .arg(layer->inputSize)
                        .arg(layer->units);
        }
        else if (layer->layerType == "Dropout") {
            code += QString("        self.dropout%1 = nn.Dropout(p=%2)\n")
                        .arg(i + 1)
                        .arg(layer->dropoutRate);
        }
    }
    // 生成前向传播函数
    code += "\n    def forward(self, x):\n";
    for (int i = 0; i < layers.size() ; ++i) {
        const NeuralLayer* layer = layers[layers.size()-1-i];
        if (layer->layerType == "Dense") {
            code += QString("        x = self.fc%1(x)\n").arg(i + 1);
            if (!layer->activationFunction.isEmpty()) {
                if (layer->activationFunction == "relu") {
                    code += "        x = nn.functional.relu(x)\n";
                } else if (layer->activationFunction == "sigmoid") {
                    code += "        x = torch.sigmoid(x)\n";
                } else if (layer->activationFunction == "softmax") {
                    code += "        x = nn.functional.softmax(x, dim=1)\n";
                } else if (layer->activationFunction == "tanh") {
                    code += "        x = torch.tanh(x)\n";
                }
            }
        } else if (layer->layerType == "Convolutional") {
            code += QString("        x = self.conv%1(x)\n").arg(i + 1);
            if (!layer->activationFunction.isEmpty()) {
                if (layer->activationFunction == "relu") {
                    code += "        x = nn.functional.relu(x)\n";
                } else if (layer->activationFunction == "sigmoid") {
                    code += "        x = torch.sigmoid(x)\n";
                } else if (layer->activationFunction == "softmax") {
                    code += "        x = nn.functional.softmax(x, dim=1)\n";
                } else if (layer->activationFunction == "tanh") {
                    code += "        x = torch.tanh(x)\n";
                }
            }
        } else if (layer->layerType == "MaxPooling" || layer->layerType == "AveragePooling") {
            code += QString("        x = self.pool%1(x)\n").arg(i + 1);
        } else if (layer->layerType == "LSTM") {
            code += QString("        x, _ = self.lstm%1(x)\n").arg(i + 1);
            if (!layer->activationFunction.isEmpty()) {
                if (layer->activationFunction == "relu") {
                    code += "        x = nn.functional.relu(x)\n";
                } else if (layer->activationFunction == "sigmoid") {
                    code += "        x = torch.sigmoid(x)\n";
                } else if (layer->activationFunction == "softmax") {
                    code += "        x = nn.functional.softmax(x, dim=1)\n";
                } else if (layer->activationFunction == "tanh") {
                    code += "        x = torch.tanh(x)\n";
                }
            }
        } else if (layer->layerType == "RNN") {
            code += QString("        x, _ = self.rnn%1(x)\n").arg(i + 1);
            if (!layer->activationFunction.isEmpty()) {
                if (layer->activationFunction == "relu") {
                    code += "        x = nn.functional.relu(x)\n";
                } else if (layer->activationFunction == "sigmoid") {
                    code += "        x = torch.sigmoid(x)\n";
                } else if (layer->activationFunction == "softmax") {
                    code += "        x = nn.functional.softmax(x, dim=1)\n";
                } else if (layer->activationFunction == "tanh") {
                    code += "        x = torch.tanh(x)\n";
                }
            }
        } else if (layer->layerType == "Dropout") {
            code += QString("        x = self.dropout%1(x)\n").arg(i + 1);
        }
    }
    code += "        return x\n\n";
    code += "model = Net()\n";

    // 定义损失函数和优化器
    code += "# 定义损失函数和优化器\n";
    code += "criterion = nn.CrossEntropyLoss()\n";
    code += "optimizer = optim.Adam(model.parameters(), lr=0.001)\n\n";

    // 训练循环
    code += "# 训练循环\n";
    code += "num_epochs = 10\n";
    code += "for epoch in range(num_epochs):\n";
    code += "    running_loss = 0.0\n";
    code += "    # 这里假设 train_loader 是你的训练数据加载器\n";
    code += "    # 你需要根据实际情况替换为你的数据加载逻辑\n";
    code += "    for i, (inputs, labels) in enumerate(train_loader):\n";
    code += "        # 清零梯度\n";
    code += "        optimizer.zero_grad()\n";
    code += "        # 前向传播\n";
    code += "        outputs = model(inputs)\n";
    code += "        # 计算损失\n";
    code += "        loss = criterion(outputs, labels)\n";
    code += "        # 反向传播和优化\n";
    code += "        loss.backward()\n";
    code += "        optimizer.step()\n";
    code += "        running_loss += loss.item()\n";
    code += "    print(f'Epoch {epoch + 1}, Loss: {running_loss / len(train_loader)}')\n";
    return code;
}


QString CodeGenerator::generateCodeFromJson(const QString& jsonStr) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isObject()) {
        qDebug() << "错误: JSON 不是对象";
        return "";
    }

    QJsonObject obj = doc.object();
    if (!obj.contains("input") ||!obj.contains("output")) {
        qDebug() << "错误: JSON 对象缺少 'input' 或 'output' 字段";
        return "";
    }

    int inputSize = obj["input"].toInt();
    int outputSize = obj["output"].toInt();

    QList<NeuralLayer*> layers;

    // 处理输入层到第一个隐藏层
    NeuralLayer* inputLayer = new NeuralLayer();
    inputLayer->layerType = "Dense";
    inputLayer->inputSize = inputSize;
    if (obj.contains("hidden") && obj["hidden"].isArray()) {
        QJsonArray hiddenArray = obj["hidden"].toArray();
        if (!hiddenArray.isEmpty()) {
            inputLayer->neurons = hiddenArray[0].toInt();
        }
    }
    layers.append(inputLayer);

    // 处理隐藏层
    if (obj.contains("hidden") && obj["hidden"].isArray()) {
        QJsonArray hiddenArray = obj["hidden"].toArray();
        for (int i = 0; i < hiddenArray.size() - 1; ++i) {
            NeuralLayer* hiddenLayer = new NeuralLayer();
            hiddenLayer->layerType = "Dense";
            hiddenLayer->inputSize = hiddenArray[i].toInt();
            hiddenLayer->neurons = hiddenArray[i + 1].toInt();
            layers.append(hiddenLayer);
        }
    }

    // 处理最后一个隐藏层到输出层
    NeuralLayer* outputLayer = new NeuralLayer();
    outputLayer->layerType = "Dense";
    if (obj.contains("hidden") && obj["hidden"].isArray()) {
        QJsonArray hiddenArray = obj["hidden"].toArray();
        if (!hiddenArray.isEmpty()) {
            outputLayer->inputSize = hiddenArray.last().toInt();
        } else {
            outputLayer->inputSize = inputSize;
        }
    }
    outputLayer->neurons = outputSize;
    layers.append(outputLayer);

    // 生成代码
    QString code = generatePyTorchCode(layers);

    // 释放内存
    for (NeuralLayer* layer : layers) {
        delete layer;
    }

    return code;
}
