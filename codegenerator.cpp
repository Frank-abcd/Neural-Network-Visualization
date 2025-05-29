#include "codegenerator.h"

QString CodeGenerator::generatePyTorchCode(const QList<NeuralLayer>& layers) {
    QString code = "# PyTorch 神经网络自动生成代码\n";
    code += "import torch\n";
    code += "import torch.nn as nn\n\n";
    code += "class Net(nn.Module):\n";
    code += "    def __init__(self):\n";
    code += "        super(Net, self).__init__()\n";

    // 生成层定义
    for (int i = 0; i < layers.size(); i++) {
        const auto& layer = layers[i];
        if (layer.layerType == "Dense") {
            code += QString("        self.fc%1 = nn.Linear(%2, %3)\n")
            .arg(i+1)
                .arg(layer.inputSize)
                .arg(layer.neurons);
        }
        // 可扩展支持 Conv2d、MaxPool2d 等
    }

    // 生成前向传播函数
    code += "\n";
    code += "    def forward(self, x):\n";
    code += "        x = x.view(-1, " + QString::number(layers[0].inputSize) + ")\n";

    for (int i = 0; i < layers.size(); i++) {
        const auto& layer = layers[i];
        if (layer.layerType == "Dense") {
            code += QString("        x = self.fc%1(x)\n").arg(i+1);
            if (!layer.activationFunction.isEmpty()) {
                if (layer.activationFunction == "relu")
                    code += "        x = nn.functional.relu(x)\n";
                else if (layer.activationFunction == "sigmoid")
                    code += "        x = torch.sigmoid(x)\n";
                else if (layer.activationFunction == "softmax")
                    code += "        x = nn.functional.softmax(x, dim=1)\n";
                else if (layer.activationFunction == "tanh")
                    code += "        x = torch.tanh(x)\n";
            }
        }
    }
    code += "        return x\n\n";
    code += "model = Net()\n";

    return code;
}
;
