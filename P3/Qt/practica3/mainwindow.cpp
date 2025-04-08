#include "mainwindow.h"
#include "ui_mainwindow.h"
QString myIP = "ws://192.168.114.40/ws";
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_webSocket = new QWebSocket();
    m_connected = false;

    // Conectar WebSocket
    connect(m_webSocket, &QWebSocket::connected, this, &MainWindow::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &MainWindow::onTextMessageReceived);
    connect(m_webSocket, &QWebSocket::errorOccurred, this, &MainWindow::onError);

    QTimer *cronos = new QTimer(this);
    cronos->start(1000);
    connect(cronos, SIGNAL(timeout()),this, SLOT(loop()));


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
    if(m_connected == false){
        m_webSocket->open(QUrl(myIP));  // Reemplaza <ESP32_IP> con la IP de tu ESP32
        m_connected = true;
    }
    sendHeartbeat();
    if(ui->checkBox->isChecked()){
        //qint64 fechaActual = QDateTime::currentSecsSinceEpoch();
        //setData(getDistancia(), fechaActual);
        getDistancia();
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
void MainWindow::auxiliar(){
    QJsonObject jsonObj;
    jsonObj["type"] = "auxiliar";
    // Convertir el objeto JSON a una cadena
    QJsonDocument jsonDoc(jsonObj);
    QString mensajeJson = jsonDoc.toJson(QJsonDocument::Compact);

    if (m_connected) {
        m_webSocket->sendTextMessage(mensajeJson);
    }
}
void MainWindow::getDistancia(){
    QJsonObject jsonObject;
    jsonObject["type"] = "ultrasonico";

    // Convertir el objeto JSON a una cadena
    QJsonDocument jsonDoc(jsonObject);
    QString mensajeJson = jsonDoc.toJson(QJsonDocument::Compact);

    if (m_connected) {
        m_webSocket->sendTextMessage(mensajeJson);
    }
}
void MainWindow::setDial(int angulo){
    QJsonObject jsonObject;
    jsonObject["type"] = "servo";  // Valor por defecto para "dispositivo"
    jsonObject["angulo"] = angulo;     // Valor de "angulo" es el valor de valorDial

    // Convertir el objeto JSON a una cadena
    QJsonDocument jsonDoc(jsonObject);
    QString mensajeJson = jsonDoc.toJson(QJsonDocument::Compact);

    if (m_connected) {
        m_webSocket->sendTextMessage(mensajeJson);
    }
}
void MainWindow::on_dial_sliderMoved(int position)
{
    ui->lcdNumber->display(position);
}


void MainWindow::on_dial_sliderReleased()
{
    int angulo = ui->dial->value();
    ui->lcdNumber->display(angulo);
    setDial(angulo);
}

void MainWindow::sendHeartbeat() {
    QJsonObject heartbeatMessage;
    heartbeatMessage["type"] = "heartbeat";
    heartbeatMessage["timestamp"] = QDateTime::currentSecsSinceEpoch();  // Unix timestamp

    QJsonDocument doc(heartbeatMessage);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    if (m_connected) {
        m_webSocket->sendTextMessage(jsonString);
        qDebug() << "Enviando heartbeat: " << jsonString;
    }
}

void MainWindow::onConnected() {
    qDebug() << "WebSocket connected!";
    m_connected = true;  // Actualizar el estado de la conexión
}

void MainWindow::onTextMessageReceived(const QString &message) {
    qDebug() << "Mensaje recibido:" << message;
    ui->textEdit->append(message);
    // Procesar el mensaje JSON
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject jsonObj = doc.object();

        if (jsonObj.contains("type") && jsonObj["type"] == "medicion_distancia") {
            double adcValue = jsonObj["distancia"].toDouble();
            qint64 currentTime = QDateTime::currentSecsSinceEpoch();
            //aqui se supone que se graficaba para lo del punto extra
            ui->textEdit->append(message);
            ui->lcdNumber_3->display(adcValue);
            setData(adcValue, currentTime);
        }
    }
}


void MainWindow::onDisconnected() {
    qDebug() << "WebSocket disconnected!";
    ui->textEdit->append("Desconectado del WebSocket");
    m_connected = false;  // Actualizar el estado de la conexión
}

void MainWindow::onError(QAbstractSocket::SocketError errores) {
    QString errorMsg;
    switch (errores) {
    case QAbstractSocket::HostNotFoundError:
        errorMsg = "Host no encontrado.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errorMsg = "Conexión rechazada.";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorMsg = "El host remoto cerró la conexión.";
        break;
    default:
        errorMsg = "Error desconocido.";
        break;
    }
    qDebug() << "Error de WebSocket: " << errorMsg;
    ui->textEdit->append("Error de WebSocket: " + errorMsg);
}

void MainWindow::connectWebSocket(const QUrl &url) {
    if (!m_connected) {
        m_webSocket->open(url);
        qDebug() << "Conectando a WebSocket en" << url;
        ui->textEdit->append("Conectando a WebSocket en: " + url.toString());
    } else {
        qDebug() << "WebSocket ya está conectado";
        ui->textEdit->append("WebSocket ya está conectado");
    }
}

void MainWindow::on_pushButton_pressed()
{
    auxiliar();

}

