#include "propertypanel.h"

PropertyPanel::PropertyPanel(QWidget *parent) : QWidget(parent)
{
    layout = new QFormLayout(this);
    updateBtn = new QPushButton("更新参数", this);
    layout->addWidget(updateBtn);

    connect(updateBtn, &QPushButton::clicked, this, &PropertyPanel::onUpdateButtonClicked);
}

void PropertyPanel::setLayerType(const QString& type)
{
    currentLayerType = type;
}

void PropertyPanel::setParameters(const QMap<QString, QString>& params)
{
    // 清空旧参数输入框
    for (auto& widget : fieldMap)
        layout->removeWidget(widget);
    fieldMap.clear();

    // 创建新的输入框
    for (auto it = params.begin(); it != params.end(); ++it) {
        QLineEdit* lineEdit = new QLineEdit(it.value(), this);
        layout->insertRow(layout->rowCount() - 1, it.key(), lineEdit);
        fieldMap[it.key()] = lineEdit;
    }
}

void PropertyPanel::onUpdateButtonClicked()
{
    QMap<QString, QString> newParams;
    for (auto it = fieldMap.begin(); it != fieldMap.end(); ++it) {
        newParams[it.key()] = it.value()->text();
    }
    emit parametersUpdated(newParams);
}
