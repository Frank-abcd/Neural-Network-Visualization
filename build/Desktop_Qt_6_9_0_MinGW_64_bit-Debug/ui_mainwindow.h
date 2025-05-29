/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *user;
    QPushButton *mode;
    QPushButton *generate_code;
    QPushButton *generate_image;
    QPushButton *history;
    QPushButton *start_new;
    QPushButton *previous;
    QPushButton *turnback;
    QPushButton *save;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(691, 513);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 0, 58, 491));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        user = new QPushButton(layoutWidget);
        user->setObjectName("user");

        verticalLayout->addWidget(user);

        mode = new QPushButton(layoutWidget);
        mode->setObjectName("mode");

        verticalLayout->addWidget(mode);

        generate_code = new QPushButton(layoutWidget);
        generate_code->setObjectName("generate_code");

        verticalLayout->addWidget(generate_code);

        generate_image = new QPushButton(layoutWidget);
        generate_image->setObjectName("generate_image");

        verticalLayout->addWidget(generate_image);

        history = new QPushButton(layoutWidget);
        history->setObjectName("history");

        verticalLayout->addWidget(history);

        start_new = new QPushButton(layoutWidget);
        start_new->setObjectName("start_new");

        verticalLayout->addWidget(start_new);

        previous = new QPushButton(layoutWidget);
        previous->setObjectName("previous");

        verticalLayout->addWidget(previous);

        turnback = new QPushButton(layoutWidget);
        turnback->setObjectName("turnback");

        verticalLayout->addWidget(turnback);

        save = new QPushButton(layoutWidget);
        save->setObjectName("save");

        verticalLayout->addWidget(save);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 691, 18));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        user->setText(QString());
        mode->setText(QString());
        generate_code->setText(QString());
        generate_image->setText(QString());
        history->setText(QString());
        start_new->setText(QString());
        previous->setText(QString());
        turnback->setText(QString());
        save->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
