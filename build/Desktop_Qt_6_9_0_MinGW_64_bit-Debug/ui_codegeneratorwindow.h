/********************************************************************************
** Form generated from reading UI file 'codegeneratorwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CODEGENERATORWINDOW_H
#define UI_CODEGENERATORWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_CodeGeneratorWindow
{
public:
    QLabel *label;
    QPushButton *return_mainwindow;

    void setupUi(QDialog *CodeGeneratorWindow)
    {
        if (CodeGeneratorWindow->objectName().isEmpty())
            CodeGeneratorWindow->setObjectName("CodeGeneratorWindow");
        CodeGeneratorWindow->resize(400, 300);
        label = new QLabel(CodeGeneratorWindow);
        label->setObjectName("label");
        label->setGeometry(QRect(108, 10, 191, 20));
        return_mainwindow = new QPushButton(CodeGeneratorWindow);
        return_mainwindow->setObjectName("return_mainwindow");
        return_mainwindow->setGeometry(QRect(300, 260, 93, 28));
        return_mainwindow->setStyleSheet(QString::fromUtf8(""));
        return_mainwindow->raise();
        label->raise();

        retranslateUi(CodeGeneratorWindow);

        QMetaObject::connectSlotsByName(CodeGeneratorWindow);
    } // setupUi

    void retranslateUi(QDialog *CodeGeneratorWindow)
    {
        CodeGeneratorWindow->setWindowTitle(QCoreApplication::translate("CodeGeneratorWindow", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("CodeGeneratorWindow", "      \347\245\236\347\273\217\347\275\221\347\273\234\346\220\255\345\273\272\345\271\263\345\217\260", nullptr));
        return_mainwindow->setText(QCoreApplication::translate("CodeGeneratorWindow", "\350\277\224\345\233\236\344\270\273\347\225\214\351\235\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CodeGeneratorWindow: public Ui_CodeGeneratorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CODEGENERATORWINDOW_H
