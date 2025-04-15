#include "mainwindow.h"
#include "ui_mainwindow.h"
QString myIP = "ws://192.168.222.40/ws";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lcdNumber->setStyleSheet("color:black");
    ui->lcdNumber_2->setStyleSheet("color:black");
    ui->lcdNumber_3->setStyleSheet("color:black");
    ui->lcdNumber_4->setStyleSheet("color:black");
    ui->lcdNumber_5->setStyleSheet("color:black");
    ui->lcdNumber_3->display(ui->dial_2->value());
    ui->lcdNumber_4->display(ui->dial_3->value());
    ui->lcdNumber_5->display(ui->dial_4->value());
    m_webSocket = new QWebSocket();
    m_connected = false;
    // Conectar WebSocket
    connect(m_webSocket, &QWebSocket::connected, this, &MainWindow::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &MainWindow::onTextMessageReceived);
    connect(m_webSocket, &QWebSocket::errorOccurred, this, &MainWindow::onError);

    if(m_connected == false){
        m_webSocket->open(QUrl(myIP));  // Reemplaza <ESP32_IP> con la IP de tu ESP32
        m_connected = true;
    }

    QTimer *cronos = new QTimer(this);
    cronos->start(1000);
    connect(cronos, SIGNAL(timeout()),this, SLOT(loop()));

    // Configuración para gráfica de distancia
    chartTemp = new QChart();
    seriesTemp = new QLineSeries();
    chartTemp->addSeries(seriesTemp);
    chartTemp->setTitle("Sensor Ultrasonico");

    axisXTemp = new QValueAxis();
    axisXTemp->setTitleText("Tiempo [s]");
    axisXTemp->setRange(0, 50);
    chartTemp->addAxis(axisXTemp, Qt::AlignBottom);
    seriesTemp->attachAxis(axisXTemp);

    // Inicializar el eje Y para distancia y configurarlo
    axisYTemp = new QValueAxis();
    axisYTemp->setTitleText("Distancia [cm]");
    axisYTemp->setRange(0, 150);  // Ajusta según el rango esperado de distancia
    chartTemp->addAxis(axisYTemp, Qt::AlignLeft);
    seriesTemp->attachAxis(axisYTemp);

    ui->widget->setChart(chartTemp);
}

MainWindow::~MainWindow()
{
    m_webSocket->close();
    delete ui;
}

void MainWindow::loop(){
    sendHeartbeat();
    if(ui->checkBox->isChecked()){
        msgsUltra();
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
        //En esta parte deben leerse las posibles respuestas JSON del servidor (ESP32)
        if (jsonObj.contains("type") && jsonObj["type"] == "medicionDistancia") {
            double disRespuesta = jsonObj["distancia"].toDouble();
            qint64 currentTime = QDateTime::currentSecsSinceEpoch();
            //aqui se supone que se graficaba para lo del punto extra
            ui->textEdit->append(message);
            ui->lcdNumber->display(disRespuesta);
            if(ui->checkBox->isChecked())
                setData(disRespuesta, currentTime);
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
void MainWindow::sendJSON(QJsonObject objJSON){
    QJsonDocument jsonDoc(objJSON);
    QString msgsJson = jsonDoc.toJson(QJsonDocument::Compact);
    if(m_connected)
        m_webSocket->sendTextMessage(msgsJson);
}
void MainWindow::moveMotor(QString direccion){
    QJsonObject jsonObj;
    jsonObj["type"] = "motor";
    jsonObj["comando"] = direccion;
    sendJSON(jsonObj);
}
void MainWindow::msgsUltra(){
    QJsonObject jsonObj;
    jsonObj["type"] = "ultrasonico";
    sendJSON(jsonObj);
}
void MainWindow::on_pushButton_clicked()//adelante
{
    moveMotor("adelante");
}


void MainWindow::on_pushButton_3_clicked()//atras
{
    moveMotor("atras");
}


void MainWindow::on_pushButton_2_clicked()//derecha
{
    moveMotor("derecha");
}


void MainWindow::on_pushButton_4_clicked()//izquierda
{
    moveMotor("izquierda");
}
void MainWindow::setDial(int angulo){
    QJsonObject jsonObj;
    jsonObj["type"] = "servo";
    jsonObj["angulo"] = angulo;
    sendJSON(jsonObj);
}

void MainWindow::on_pushButton_6_clicked()
{
    msgsUltra();

}
void MainWindow::on_dial_sliderMoved(int position)
{
    ui->lcdNumber_2->display(position);
}


void MainWindow::on_dial_sliderReleased()
{
    int angulo = ui->dial->value();
    ui->lcdNumber_2->display(angulo);
    setDial(angulo);
}


void MainWindow::on_pushButton_7_clicked()
{
    m_webSocket->close();
    m_connected = false;
    if(m_connected == false){
        m_webSocket->open(QUrl(myIP));  // Reemplaza <ESP32_IP> con la IP de tu ESP32
        m_connected = true;
    }
}


void MainWindow::on_pushButton_5_clicked()
{
    moveMotor("stop");
}


void MainWindow::on_pushButton_8_pressed()
{
    QJsonObject jsonObj;
    jsonObj["type"] = "busqueda";
    QString obj = "\""+ QString::number(ui->comboBox->currentIndex()) +"\"";
    jsonObj["objeto"] = obj;
    sendJSON(jsonObj);
}


void MainWindow::on_dial_2_sliderMoved(int position)
{
    ui->lcdNumber_3->display(position);
}


void MainWindow::on_dial_2_sliderReleased()
{
    int valor = ui->dial_2->value();
    ui->lcdNumber_3->display(valor);
}


void MainWindow::on_dial_3_sliderMoved(int position)
{
    ui->lcdNumber_4->display(position);
}


void MainWindow::on_dial_3_sliderReleased()
{
    int valor = ui->dial_3->value();
    ui->lcdNumber_4->display(valor);
}


void MainWindow::on_dial_4_sliderMoved(int position)
{
    ui->lcdNumber_5->display(position);
}


void MainWindow::on_dial_4_sliderReleased()
{
    int valor = ui->dial_4->value();
    ui->lcdNumber_5->display(valor);
}


void MainWindow::on_pushButton_9_clicked()
{

    QJsonObject jsonObj;
    jsonObj["type"] = "setTimeMotors";
    jsonObj["adelante"] = QString::number(ui->dial_2->value());
    jsonObj["atras"] = QString::number(ui->dial_3->value());
    jsonObj["laterales"] = QString::number(ui->dial_4->value());
    //Para mostrar el objeto json es necesario convertirlo en una cadena
    QJsonDocument jsonDoc(jsonObj);
    QString mensajeJson = jsonDoc.toJson(QJsonDocument::Compact);
    qDebug() << mensajeJson;
    sendJSON(jsonObj);
}

