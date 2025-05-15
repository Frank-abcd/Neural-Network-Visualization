/*#include "codegenerator.h"

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
}*/
#include "codegenerator.h"
#include <QList>

QString CodeGenerator::generatePyTorchCode(const QList<NeuralLayer*>& layers) {
    QString code = "# PyTorch 神经网络自动生成代码\n";
    code += "import torch\n";
    code += "import torch.nn as nn\n";
    code += "import torch.nn.functional as F\n\n";
    code += "class Net(nn.Module):\n";
    code += "    def __init__(self):\n";
    code += "        super(Net, self).__init__()\n";

    bool hasConvOrPool = false;
    bool hasRNN = false;
    int convCount = 1;
    int rnnCount = 1;
    int denseCount = 1;

    // 生成层定义
    for (int i = 0; i < layers.size(); i++) {
        NeuralLayer* layer = layers[i];

        if (layer->layerType == "Dense") {
            DenseLayer* denseLayer = dynamic_cast<DenseLayer*>(layer);
            if (denseLayer) {
                code += QString("        self.fc%1 = nn.Linear(%2, %3)\n")
                       .arg(denseCount++)
                       .arg(denseLayer->inputSize)
                       .arg(denseLayer->neurons);
            }
        }
        else if (layer->layerType == "Conv2d") {
            Conv2dLayer* convLayer = dynamic_cast<Conv2dLayer*>(layer);
            if (convLayer) {
                hasConvOrPool = true;
                code += QString("        self.conv%1 = nn.Conv2d(%2, %3, kernel_size=%4, stride=%5, padding=%6)\n")
                           .arg(convCount++)
                           .arg(convLayer->inputChannels)
                           .arg(convLayer->outputChannels)
                           .arg(convLayer->kernelSize)
                           .arg(convLayer->stride)
                           .arg(convLayer->padding);
            }
        }
        else if (layer->layerType == "MaxPool2d") {
            MaxPool2dLayer* poolLayer = dynamic_cast<MaxPool2dLayer*>(layer);
            if (poolLayer) {
                hasConvOrPool = true;
                code += QString("        self.pool%1 = nn.MaxPool2d(kernel_size=%2, stride=%3, padding=%4)\n")
                           .arg(convCount)
                           .arg(poolLayer->kernelSize)
                           .arg(poolLayer->stride)
                           .arg(poolLayer->padding);
            }
        }
        else if (layer->layerType == "AvgPool2d") {
            AvgPool2dLayer* poolLayer = dynamic_cast<AvgPool2dLayer*>(layer);
            if (poolLayer) {
                hasConvOrPool = true;
                code += QString("        self.pool%1 = nn.AvgPool2d(kernel_size=%2, stride=%3, padding=%4)\n")
                           .arg(convCount)
                           .arg(poolLayer->kernelSize)
                           .arg(poolLayer->stride)
                           .arg(poolLayer->padding);
            }
        }
        else if (layer->layerType == "LSTM") {
            LSTMLayer* lstmLayer = dynamic_cast<LSTMLayer*>(layer);
            if (lstmLayer) {
                hasRNN = true;
                code += QString("        self.lstm%1 = nn.LSTM(input_size=%2, hidden_size=%3, num_layers=%4, batch_first=%5, bidirectional=%6)\n")
                           .arg(rnnCount++)
                           .arg(lstmLayer->inputSize)
                           .arg(lstmLayer->hiddenSize)
                           .arg(lstmLayer->numLayers)
                           .arg(lstmLayer->batchFirst ? "True" : "False")
                           .arg(lstmLayer->bidirectional ? "True" : "False");
            }
        }
        else if (layer->layerType == "GRU") {
            GRULayer* gruLayer = dynamic_cast<GRULayer*>(layer);
            if (gruLayer) {
                hasRNN = true;
                code += QString("        self.gru%1 = nn.GRU(input_size=%2, hidden_size=%3, num_layers=%4, batch_first=%5, bidirectional=%6)\n")
                           .arg(rnnCount++)
                           .arg(gruLayer->inputSize)
                           .arg(gruLayer->hiddenSize)
                           .arg(gruLayer->numLayers)
                           .arg(gruLayer->batchFirst ? "True" : "False")
                           .arg(gruLayer->bidirectional ? "True" : "False");
            }
        }
        else if (layer->layerType == "RNN") {
            RNNLayer* rnnLayer = dynamic_cast<RNNLayer*>(layer);
            if (rnnLayer) {
                hasRNN = true;
                code += QString("        self.rnn%1 = nn.RNN(input_size=%2, hidden_size=%3, num_layers=%4, batch_first=%5, bidirectional=%6, nonlinearity='%7')\n")
                           .arg(rnnCount++)
                           .arg(rnnLayer->inputSize)
                           .arg(rnnLayer->hiddenSize)
                           .arg(rnnLayer->numLayers)
                           .arg(rnnLayer->batchFirst ? "True" : "False")
                           .arg(rnnLayer->bidirectional ? "True" : "False")
                           .arg(rnnLayer->rnnActivation);
            }
        }
        else if (layer->layerType == "Dropout") {
            DropoutLayer* dropoutLayer = dynamic_cast<DropoutLayer*>(layer);
            if (dropoutLayer) {
                code += QString("        self.dropout%1 = nn.Dropout(p=%2)\n")
                       .arg(i+1)
                       .arg(dropoutLayer->dropoutRate);
            }
        }
        else if (layer->layerType == "BatchNorm2d") {
            BatchNorm2dLayer* bnLayer = dynamic_cast<BatchNorm2dLayer*>(layer);
            if (bnLayer) {
                code += QString("        self.bn%1 = nn.BatchNorm2d(%2)\n")
                       .arg(convCount)
                       .arg(bnLayer->numFeatures);
            }
        }
        else if (layer->layerType == "BatchNorm1d") {
            BatchNorm1dLayer* bnLayer = dynamic_cast<BatchNorm1dLayer*>(layer);
            if (bnLayer) {
                code += QString("        self.bn%1 = nn.BatchNorm1d(%2)\n")
                       .arg(denseCount)
                       .arg(bnLayer->numFeatures);
            }
        }
        else if (layer->layerType == "Flatten") {
            code += "        self.flatten = nn.Flatten()\n";
        }
    }

    // 生成前向传播函数
    code += "\n";
    code += "    def forward(self, x):\n";

    // 处理卷积/池化层
    if (hasConvOrPool) {
        code += "        # 卷积/池化层处理\n";
    }

    bool needFlatten = false;
    denseCount = 1;
    convCount = 1;
    rnnCount = 1;

    for (int i = 0; i < layers.size(); i++) {
        NeuralLayer* layer = layers[i];

        if (layer->layerType == "Dense") {
            DenseLayer* denseLayer = dynamic_cast<DenseLayer*>(layer);
            if (denseLayer) {
                if (i > 0 && (layers[i-1]->layerType == "Conv2d" || layers[i-1]->layerType == "MaxPool2d" ||
                              layers[i-1]->layerType == "AvgPool2d" || layers[i-1]->layerType == "Flatten")) {
                    if (!needFlatten) {
                        code += "        x = x.view(x.size(0), -1)  # 展平\n";
                        needFlatten = true;
                    }
                }
                code += QString("        x = self.fc%1(x)\n").arg(denseCount++);
            }
        }
        else if (layer->layerType == "Conv2d") {
            Conv2dLayer* convLayer = dynamic_cast<Conv2dLayer*>(layer);
            if (convLayer) {
                code += QString("        x = self.conv%1(x)\n").arg(convCount++);
                needFlatten = false;
            }
        }
        else if (layer->layerType == "MaxPool2d" || layer->layerType == "AvgPool2d") {
            code += QString("        x = self.pool%1(x)\n").arg(convCount);
            needFlatten = false;
        }
        else if (layer->layerType == "LSTM") {
            LSTMLayer* lstmLayer = dynamic_cast<LSTMLayer*>(layer);
            if (lstmLayer) {
                code += QString("        x, (h_n, c_n) = self.lstm%1(x)\n").arg(rnnCount++);
                if (lstmLayer->returnSequence) {
                    code += "        # 返回整个序列\n";
                } else {
                    code += "        # 只返回最后一个时间步的输出\n";
                    code += "        x = x[:, -1, :]\n";
                }
            }
        }
        else if (layer->layerType == "GRU") {
            GRULayer* gruLayer = dynamic_cast<GRULayer*>(layer);
            if (gruLayer) {
                code += QString("        x, h_n = self.gru%1(x)\n").arg(rnnCount++);
                if (gruLayer->returnSequence) {
                    code += "        # 返回整个序列\n";
                } else {
                    code += "        # 只返回最后一个时间步的输出\n";
                    code += "        x = x[:, -1, :]\n";
                }
            }
        }
        else if (layer->layerType == "RNN") {
            RNNLayer* rnnLayer = dynamic_cast<RNNLayer*>(layer);
            if (rnnLayer) {
                code += QString("        x, h_n = self.rnn%1(x)\n").arg(rnnCount++);
                if (rnnLayer->returnSequence) {
                    code += "        # 返回整个序列\n";
                } else {
                    code += "        # 只返回最后一个时间步的输出\n";
                    code += "        x = x[:, -1, :]\n";
                }
            }
        }
        else if (layer->layerType == "Dropout") {
            DropoutLayer* dropoutLayer = dynamic_cast<DropoutLayer*>(layer);
            if (dropoutLayer) {
                code += QString("        x = self.dropout%1(x)\n").arg(i+1);
            }
        }
        else if (layer->layerType == "BatchNorm2d") {
            BatchNorm2dLayer* bnLayer = dynamic_cast<BatchNorm2dLayer*>(layer);
            if (bnLayer) {
                code += QString("        x = self.bn%1(x)\n").arg(convCount);
            }
        }
        else if (layer->layerType == "BatchNorm1d") {
            BatchNorm1dLayer* bnLayer = dynamic_cast<BatchNorm1dLayer*>(layer);
            if (bnLayer) {
                code += QString("        x = self.bn%1(x)\n").arg(denseCount);
            }
        }
        else if (layer->layerType == "Flatten") {
            code += "        x = self.flatten(x)\n";
            needFlatten = true;
        }

        // 激活函数处理
        if (!layer->activationFunction.isEmpty()) {
            if (layer->activationFunction == "relu")
                code += "        x = F.relu(x)\n";
            else if (layer->activationFunction == "sigmoid")
                code += "        x = torch.sigmoid(x)\n";
            else if (layer->activationFunction == "softmax")
                code += "        x = F.softmax(x, dim=1)\n";
            else if (layer->activationFunction == "tanh")
                code += "        x = torch.tanh(x)\n";
            else if (layer->activationFunction == "leaky_relu")
                code += QString("        x = F.leaky_relu(x, negative_slope=%1)\n").arg(layer->leakyReluSlope);
        }
    }

    code += "        return x\n\n";

    // 添加模型初始化代码
    code += "# 模型初始化\n";
    code += "model = Net()\n\n";

    // 添加设备选择代码
    code += "# 设备选择\n";
    code += "device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')\n";
    code += "model = model.to(device)\n\n";

    // 添加模型摘要打印函数
    code += "# 打印模型摘要\n";
    code += "def model_summary(model):\n";
    code += "    print(\"Model structure:\")\n";
    code += "    print(model)\n";
    code += "    print(\"\\nParameters:\")\n";
    code += "    total_params = sum(p.numel() for p in model.parameters())\n";
    code += "    print(f\"Total parameters: {total_params:,}\")\n";
    code += "    trainable_params = sum(p.numel() for p in model.parameters() if p.requires_grad)\n";
    code += "    print(f\"Trainable parameters: {trainable_params:,}\")\n\n";
    code += "model_summary(model)\n";

    return code;
}
