#include "codegenerator.h"
#include <QList>

QString CodeGenerator::generatePyTorchCode(const QList<NeuralLayer>& layers) {
    QString code = "# PyTorch 神经网络自动生成代码\n";
    code += "import torch\n";
    code += "import torch.nn as nn\n\n";
    code += "class Net(nn.Module):\n";
    code += "    def __init__(self):\n";
    code += "        super(Net, self).__init__()\n";

    // 生成层定义
    for (int i = 0; i < layers.size(); ++i) {
        const NeuralLayer& layer = layers[i];
        if (layer.layerType == "Dense") {
            code += QString("        self.fc%1 = nn.Linear(%2, %3)\n")
            .arg(i + 1)
                .arg(layer.inputSize)
                .arg(layer.neurons);
        } else if (layer.layerType == "Convolutional") {
            code += QString("        self.conv%1 = nn.Conv2d(%2, %3, kernel_size=3)\n")
            .arg(i + 1)
                .arg(layer.inputSize)
                .arg(layer.neurons);
        } else if (layer.layerType == "Pooling") {
            code += QString("        self.pool%1 = nn.MaxPool2d(kernel_size=2)\n")
            .arg(i + 1);
        } else if (layer.layerType == "LSTM") {
            code += QString("        self.lstm%1 = nn.LSTM(%2, %3)\n")
            .arg(i + 1)
                .arg(layer.inputSize)
                .arg(layer.neurons);
        } else if (layer.layerType == "RNN") {
            code += QString("        self.rnn%1 = nn.RNN(%2, %3)\n")
            .arg(i + 1)
                .arg(layer.inputSize)
                .arg(layer.neurons);
        } else if (layer.layerType == "Dropout") {
            code += QString("        self.dropout%1 = nn.Dropout(p=0.5)\n")
            .arg(i + 1);
        }
    }

    // 生成前向传播函数
    code += "\n    def forward(self, x):\n";
    for (int i = 0; i < layers.size(); ++i) {
        const NeuralLayer& layer = layers[i];
        if (layer.layerType == "Dense") {
            code += QString("        x = self.fc%1(x)\n").arg(i + 1);
            if (!layer.activationFunction.isEmpty()) {
                if (layer.activationFunction == "relu") {
                    code += "        x = nn.functional.relu(x)\n";
                } else if (layer.activationFunction == "sigmoid") {
                    code += "        x = torch.sigmoid(x)\n";
                } else if (layer.activationFunction == "softmax") {
                    code += "        x = nn.functional.softmax(x, dim=1)\n";
                } else if (layer.activationFunction == "tanh") {
                    code += "        x = torch.tanh(x)\n";
                }
            }
        } else if (layer.layerType == "Convolutional") {
            code += QString("        x = self.conv%1(x)\n").arg(i + 1);
            // 卷积层后通常接激活函数，但根据需求不添加
        } else if (layer.layerType == "Pooling") {
            code += QString("        x = self.pool%1(x)\n").arg(i + 1);
        } else if (layer.layerType == "LSTM") {
            code += QString("        x, _ = self.lstm%1(x)\n").arg(i + 1);
        } else if (layer.layerType == "RNN") {
            code += QString("        x, _ = self.rnn%1(x)\n").arg(i + 1);
        } else if (layer.layerType == "Dropout") {
            code += QString("        x = self.dropout%1(x)\n").arg(i + 1);
        }
    }
    code += "        return x\n\n";
    code += "model = Net()\n";

    return code;
}
