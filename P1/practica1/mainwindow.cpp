#include "mainwindow.h"
#include "ui_mainwindow.h"
QString myIP = "http://192.168.100.164", myRed = "wlo1";
int contador = 0;
LEDsControl myLED = {18, 3, 9, 11, 13};
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->setText("Erick Emmanuel Grimaldo Arteaga");
    ui->label_2->setText("Dial Servo 0-180°");
    ui->label_4->setText("Contador 5 bits - 5 LEDs");
    setCerosLED();
    for(int x=0; x<5; x++)
        printStateButton(x, 0);
    ui->lcdNumber_2->setDigitCount(5);
    ui->lcdNumber_2->setMode(QLCDNumber::Bin);//LCDnumber muestra en binario
    ui->lcdNumber_2->setStyleSheet("color:red;");
    ui->lcdNumber->setStyleSheet("color:black");
    ui->label_5->setAlignment(Qt::AlignCenter);
    ui->pushButton_6->setStyleSheet(btnGeneral);
}
MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::setStateLED(int LED, int bnd){
    conexionWeb(myIP, myLED.stateLED(LED, bnd), myRed);
    printStateButton(LED, bnd);

}
void MainWindow::setCerosLED(){
    for(int i=0; i<5; i++)
        if(myLED.getState(i) == 1)
            setStateLED(i, 0);
}
void MainWindow::printStateButton(int LED, int bnd){
    switch (LED) {
    case 0:
        if(bnd == 1)
            ui->pushButton->setStyleSheet(activado);
        else
            ui->pushButton->setStyleSheet(desactivado);
        break;
    case 1:
        if(bnd == 1)
            ui->pushButton_2->setStyleSheet(activado);
        else
            ui->pushButton_2->setStyleSheet(desactivado);
        break;
    case 2:
        if(bnd == 1)
            ui->pushButton_3->setStyleSheet(activado);
        else
            ui->pushButton_3->setStyleSheet(desactivado);
        break;
    case 3:
        if(bnd == 1)
            ui->pushButton_4->setStyleSheet(activado);
        else
            ui->pushButton_4->setStyleSheet(desactivado);
        break;
    case 4:
        if(bnd == 1)
            ui->pushButton_5->setStyleSheet(activado);
        else
            ui->pushButton_5->setStyleSheet(desactivado);
        break;
    }
}
void MainWindow::_5BITS(int decimal){
    int binario[5];
    for(int x=4; x>=0; x--){
        binario[x] = decimal % 2;
        decimal /= 2;
    }
    for(int i=0; i<5; i++)
        if(myLED.getState(i) != binario[i])
            setStateLED(i, binario[i]);
}

void MainWindow::on_pushButton_clicked(bool checked){
    if(checked)
        setStateLED(0, 1);
    else
        setStateLED(0, 0);
}

void MainWindow::on_pushButton_2_clicked(bool checked)
{
    if(checked)
        setStateLED(1 ,1);
    else
        setStateLED(1, 0);
}

void MainWindow::on_pushButton_3_clicked(bool checked)
{
    if(checked)
        setStateLED(2, 1);
    else
        setStateLED(2, 0);
}

void MainWindow::on_pushButton_4_clicked(bool checked)
{
    if(checked)
        setStateLED(3, 1);
    else
        setStateLED(3, 0);
}

void MainWindow::on_pushButton_5_clicked(bool checked)
{
    if(checked)
        setStateLED(4, 1);
    else
        setStateLED(4, 0);
}

void MainWindow::on_dial_sliderMoved(int position)
{
    ui->lcdNumber->display(position);
}

void MainWindow::on_dial_sliderReleased()
{
    int angulo = ui->dial->value();
    ui->lcdNumber->display(angulo);
    // Crear el objeto JSON
    QJsonObject jsonObject;
    jsonObject["dispositivo"] = "servo";  // Valor por defecto para "dispositivo"
    jsonObject["angulo"] = angulo;     // Valor de "angulo" es el valor de valorDial

    // Convertir el objeto JSON a una cadena
    QJsonDocument jsonDoc(jsonObject);
    QString mensajeJson = jsonDoc.toJson(QJsonDocument::Compact);
    ui->label_3->setText(mensajeJson);

    conexionWeb(myIP, mensajeJson, myRed);
}

void MainWindow::on_pushButton_6_clicked()
{
    contador++;
    ui->lcdNumber_2->display(contador);

    QString aux = QString::number(contador);
    qDebug() <<aux;

    if(contador < 32)// pq son 5 bits
        _5BITS(contador);
}

void MainWindow::on_pushButton_7_clicked()
{
    contador = 0;
    ui->lcdNumber_2->display(contador);
    setCerosLED();
    ui->lcdNumber->display(0);
    ui->dial->setValue(0);
    on_dial_sliderReleased();
}

void MainWindow::on_pushButton_8_clicked()
{
    contador = myLED.getDecimal();
    for(int i=0; i<5; i++)
        qDebug() << QString::number(myLED.getState(i));
    ui->label_5->setNum(myLED.getStates());
    ui->label_6->setText(" =   " + QString::number(contador));
    ui->lcdNumber_2->display(contador);
}

