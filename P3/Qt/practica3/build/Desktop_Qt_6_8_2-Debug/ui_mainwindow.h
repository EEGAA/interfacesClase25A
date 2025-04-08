/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QCheckBox *checkBox;
    QLabel *label_3;
    QLabel *label_2;
    QComboBox *comboBox;
    QLCDNumber *lcdNumber;
    QLabel *label;
    QLCDNumber *lcdNumber_3;
    QPushButton *pushButton;
    QDial *dial;
    QChartView *widget;
    QLCDNumber *lcdNumber_2;
    QTextEdit *textEdit;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        checkBox = new QCheckBox(centralwidget);
        checkBox->setObjectName("checkBox");
        checkBox->setGeometry(QRect(20, 430, 141, 23));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(230, 410, 121, 17));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(380, 460, 31, 31));
        QFont font;
        font.setPointSize(20);
        label_2->setFont(font);
        comboBox = new QComboBox(centralwidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName("comboBox");
        comboBox->setGeometry(QRect(540, 430, 131, 25));
        lcdNumber = new QLCDNumber(centralwidget);
        lcdNumber->setObjectName("lcdNumber");
        lcdNumber->setGeometry(QRect(290, 440, 91, 71));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(420, 410, 111, 17));
        lcdNumber_3 = new QLCDNumber(centralwidget);
        lcdNumber_3->setObjectName("lcdNumber_3");
        lcdNumber_3->setGeometry(QRect(420, 440, 91, 71));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(560, 480, 89, 25));
        dial = new QDial(centralwidget);
        dial->setObjectName("dial");
        dial->setGeometry(QRect(180, 430, 111, 91));
        dial->setMaximum(180);
        widget = new QChartView(centralwidget);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(9, 0, 781, 401));
        lcdNumber_2 = new QLCDNumber(centralwidget);
        lcdNumber_2->setObjectName("lcdNumber_2");
        lcdNumber_2->setGeometry(QRect(690, 430, 91, 91));
        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(10, 529, 771, 61));
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        checkBox->setText(QCoreApplication::translate("MainWindow", "Activar Medicion", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Dial Servo 0-180\302\260", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "  =", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("MainWindow", "objeto cercano", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("MainWindow", "objeto lejano", nullptr));

        label->setText(QCoreApplication::translate("MainWindow", "Distancia [cm]", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "Buscar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
