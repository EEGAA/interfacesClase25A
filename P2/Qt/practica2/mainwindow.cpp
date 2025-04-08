#include "mainwindow.h"
#include "ui_mainwindow.h"

QString myIP = "http://192.168.114.40", myRed = "wlo1";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lcdNumber->setStyleSheet("color:black");
    ui->lcdNumber_2->setStyleSheet("color:black");
    ui->lcdNumber_3->setStyleSheet("color:black");

    QTimer *cronometro = new QTimer(this);
    cronometro->start(1000);
    connect(cronometro, SIGNAL(timeout()), this, SLOT(loop()));
    startTime = QDateTime::currentSecsSinceEpoch();
    // Configuración para gráfica de temperatura
    chartTemp = new QChart();
    seriesTemp = new QLineSeries();
    chartTemp->addSeries(seriesTemp);
    chartTemp->setTitle("Sensor Ultrasonico");

    axisXTemp = new QValueAxis();
    axisXTemp->setTitleText("Tiempo [s]");
    axisXTemp->setRange(0, 50);
    chartTemp->addAxis(axisXTemp, Qt::AlignBottom);
    seriesTemp->attachAxis(axisXTemp);

    // Inicializar el eje Y para temperatura y configurarlo
    axisYTemp = new QValueAxis();
    axisYTemp->setTitleText("Distancia [cm]");
    axisYTemp->setRange(0, 100);  // Ajusta según el rango esperado de temperatura
    chartTemp->addAxis(axisYTemp, Qt::AlignLeft);
    seriesTemp->attachAxis(axisYTemp);

    ui->widget->setChart(chartTemp);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loop(){
    if(ui->checkBox->isChecked()){
        qint64 fechaActual = QDateTime::currentSecsSinceEpoch();
        setData(getDistancia(), fechaActual);
    }
}

void MainWindow::setData(double temp, qint64 timestamp){
    qint64 currentTime = timestamp - startTime;
    dataPointsTemp.append(QPoint(currentTime, temp));
    while(!dataPointsTemp.isEmpty() && currentTime - dataPointsTemp.first().x() > 60){
        dataPointsTemp.removeFirst();
    }
    seriesTemp->replace(dataPointsTemp);
    axisXTemp->setRange(currentTime - 60, currentTime);
    axisYTemp->setRange(0, 100);
}

float MainWindow::getDistancia(){
    float distancia = 0;
    //QString aux = conexionWeb(myIP, "distancia" , myRed);
    //qDebug() << aux;
    QJsonObject jsonMensaje;
    jsonMensaje["dispositivo"] = "distancia";

    // Convertir a QByteArray
    QJsonDocument doc(jsonMensaje);
    QString mensajeJson2 = doc.toJson(QJsonDocument::Compact);
    QString respuesta = conexionWeb(myIP, mensajeJson2, myRed);
    if (!respuesta.startsWith("Error")) {//Si no hay error de conexion
        QJsonDocument docRespuesta = QJsonDocument::fromJson(respuesta.toUtf8());
        QJsonObject objRespuesta = docRespuesta.object();

        if (objRespuesta.contains("distancia")) {
            distancia = objRespuesta["distancia"].toDouble();
            qDebug() << "Distancia medida:" << distancia << "cm";
            ui->lcdNumber_3->display(distancia);
        } else {
            qDebug() << "Respuesta inesperada:" << respuesta;
        }
    } else {
        qDebug() << "Error en la conexión:" << respuesta;
    }
    return distancia;
}
void MainWindow::setDial(int angulo){
    // Crear el objeto JSON
    QJsonObject jsonObject;
    jsonObject["dispositivo"] = "servo";  // Valor por defecto para "dispositivo"
    jsonObject["angulo"] = angulo;     // Valor de "angulo" es el valor de valorDial

    // Convertir el objeto JSON a una cadena
    QJsonDocument jsonDoc(jsonObject);
    QString mensajeJson = jsonDoc.toJson(QJsonDocument::Compact);
    conexionWeb(myIP, mensajeJson, myRed);
}

void MainWindow::on_dial_sliderMoved(int position)
{
    ui->lcdNumber->display(position);
}


void MainWindow::on_dial_sliderReleased(){
    int angulo = ui->dial->value();
    ui->lcdNumber->display(angulo);
    setDial(angulo);

}


void MainWindow::on_pushButton_pressed()
{
    float busqueda;
    QJsonObject jsonMensaje;
    jsonMensaje["dispositivo"] = "distancia";
    //comboBox tiene dos posiciones en 0 para objetos cercanos y 1 para lejanos, dicho valor
    //sera aprovechado y enviado para el procesamiento en la ESP32
    QString bus = QString::number(ui->comboBox->currentIndex());
    // Convertir a QByteArray
    QString mensajeJson3 = "{\"dispositivo\":\"auxiliar\",\"obj\":" + bus +  "}";
    QString respuesta = conexionWeb(myIP, mensajeJson3 , myRed);
    if (!respuesta.startsWith("Error")) {//Si no hay error de conexion
        QJsonDocument docRespuesta = QJsonDocument::fromJson(respuesta.toUtf8());
        QJsonObject objRespuesta = docRespuesta.object();

        if (objRespuesta.contains("distancia")) {
            busqueda = objRespuesta["distancia"].toDouble();
            ui->lcdNumber_2->display(busqueda);
            qDebug() << "Distancia encontrada:" << busqueda << "cm";
        } else {
            qDebug() << "Respuesta inesperada:" << respuesta;
        }
        if(objRespuesta.contains("posicion")){//voltea a verlo
            qDebug() << respuesta;
            int angulo = objRespuesta["posicion"].toInt();
            ui->lcdNumber->display(angulo);
            ui->dial->setValue(angulo);
            setDial(angulo);
        }
    } else {
        qDebug() << "Error en la conexión:" << respuesta;
    }
    //conexionWeb(myIP, "{\"dispositivo\":\"auxiliar\"}" , myRed);
}


void MainWindow::on_pushButton_2_pressed()
{
    for(int i = 0; i <= 180; i += 20){
        setDial(i);
        qint64 fechaActual = QDateTime::currentSecsSinceEpoch();
        setData(getDistancia(), fechaActual);
        //delay(50);
    }
    /*for(int j = 180; j >= 0; j -= 10){
        setDial(j);
        //delay(50);
    }*/
    setDial(0);
}

