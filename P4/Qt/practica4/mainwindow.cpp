#include "mainwindow.h"
#include "ui_mainwindow.h"
//QString myIP = "ws://192.168.100.164/ws";
QString myIP = "192.168.100.164";
QString IPws = "ws://" + myIP + "/ws";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);  // Acepta eventos de teclado
    ui->lcdNumber->setStyleSheet("color:purple");
    ui->lcdNumber_2->setStyleSheet("color:black");
    ui->lcdNumber_3->setStyleSheet("color:black");
    ui->lcdNumber_4->setStyleSheet("color:black");
    ui->lcdNumber_5->setStyleSheet("color:black");
    ui->lcdNumber_3->display(ui->dial_2->value());
    ui->lcdNumber_4->display(ui->dial_3->value());
    ui->lcdNumber_5->display(ui->dial_4->value());

    ui->dial->setStyleSheet(setStyleDialServo);
    ui->dial_2->setStyleSheet(setStyleDialTime);
    ui->dial_3->setStyleSheet(setStyleDialTime);
    ui->dial_4->setStyleSheet(setStyleDialTime);
    ui->comboBox->setStyleSheet(setStyleMyComboBox);
    ui->pushButton_6->setStyleSheet(preSetButton);
    ui->pushButton_7->setStyleSheet(setReconectButton);
    ui->pushButton_8->setStyleSheet(preSetButton);
    ui->pushButton_9->setStyleSheet(preSetButton);
    ui->pushButton_10->setStyleSheet(preSetTimeButton);
    ui->pushButton->setStyleSheet(styleButtonDir);
    ui->pushButton_2->setStyleSheet(styleButtonDir);
    ui->pushButton_3->setStyleSheet(styleButtonDir);
    ui->pushButton_4->setStyleSheet(styleButtonDir);
    ui->pushButton_5->setStyleSheet(styleButtonStop);
    ui->checkBox->setStyleSheet(checkboxStyle);
    ui->label_3->setStyleSheet(styleLabelTime);
    ui->label_7->setStyleSheet(styleLabelTime);
    ui->lcdNumber_3->setStyleSheet(styleLCDnumberTimes);
    ui->lcdNumber_4->setStyleSheet(styleLCDnumberTimes);
    ui->lcdNumber_5->setStyleSheet(styleLCDnumberTimes);

    m_webSocket = new QWebSocket();
    m_connected = false;
    // Conectar WebSocket
    connect(m_webSocket, &QWebSocket::connected, this, &MainWindow::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &MainWindow::onTextMessageReceived);
    connect(m_webSocket, &QWebSocket::errorOccurred, this, &MainWindow::onError);

    if(m_connected == false){
        m_webSocket->open(QUrl(IPws));
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

    preSetTimeMotors();
}

MainWindow::~MainWindow()
{
    m_webSocket->close();
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    int key = event->key();//Aqui se obtiene el codigo de la tecla pulsada
    switch(key){
    case Qt::Key_W:
        moveMotor("adelante");
        qDebug()<<"tecla W";
        break;
    case Qt::Key_S:
        moveMotor("atras");
        qDebug()<<"tecla S";
        break;
    case Qt::Key_D:
        moveMotor("derecha");
        qDebug()<<"tecla D";
        break;
    case Qt::Key_A:
        moveMotor("izquierda");
        qDebug()<<"tecla A";
        break;
    case Qt::Key_Space:
        moveMotor("stop");
        qDebug()<<"tecla space";
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }

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
    qDebug()<<"El mensaja enviado: " + msgsJson;
}
void MainWindow::moveMotor(QString direccion){//direccion solo puede ser una de estas "atras, adelante, derecha, izquierda, stop"
    QJsonObject jsonObj;
    jsonObj["type"] = "motor";
    jsonObj["comando"] = direccion;
    sendJSON(jsonObj);
}
void MainWindow::msgsUltra(){//Utilizada cada que se requiere la distancia del sensor
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
void MainWindow::setDial(int angulo){//utilizada cada que se requiere mover el servo
    QJsonObject jsonObj;
    jsonObj["type"] = "servo";
    jsonObj["angulo"] = 180 - angulo;
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


void MainWindow::on_pushButton_7_clicked()//poder reconectar sin tener que cerrar y abrir la intrafaz
{
    m_webSocket->close();
    m_connected = false;
    if(m_connected == false){
        m_webSocket->open(QUrl(IPws));
        m_connected = true;
    }
}


void MainWindow::on_pushButton_5_clicked()
{
    moveMotor("stop");
}


void MainWindow::on_pushButton_8_pressed()//Segun el comboBox busca el objeto mas cercano o el mas lejano
{
    QJsonObject jsonObj;
    jsonObj["type"] = "busqueda";
    jsonObj["objeto"] = QString::number(ui->comboBox->currentIndex());
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
    ui->lcdNumber_3->setStyleSheet(styleLCDnumberTimes2);
}


void MainWindow::on_dial_3_sliderMoved(int position)
{
    ui->lcdNumber_4->display(position);
}


void MainWindow::on_dial_3_sliderReleased()
{
    int valor = ui->dial_3->value();
    ui->lcdNumber_4->display(valor);
    ui->lcdNumber_4->setStyleSheet(styleLCDnumberTimes2);
}


void MainWindow::on_dial_4_sliderMoved(int position)
{
    ui->lcdNumber_5->display(position);
}


void MainWindow::on_dial_4_sliderReleased()
{
    int valor = ui->dial_4->value();
    ui->lcdNumber_5->display(valor);
    ui->lcdNumber_5->setStyleSheet(styleLCDnumberTimes2);
}

void MainWindow::setTimeMotors(QString adelante, QString atras, QString laterales){
    QJsonObject jsonObj;
    jsonObj["type"] = "setTimeMotors";
    jsonObj["adelante"] = adelante;
    jsonObj["atras"] = atras;
    jsonObj["laterales"] = laterales;
    /*//Para mostrar el objeto json es necesario convertirlo en una cadena
    QJsonDocument jsonDoc(jsonObj);
    QString mensajeJson = jsonDoc.toJson(QJsonDocument::Compact);
    qDebug() << mensajeJson;*/
    sendJSON(jsonObj);
}

void MainWindow::on_pushButton_9_clicked()
{
    QString a, b, c;
    a = QString::number(ui->dial_2->value());
    b = QString::number(ui->dial_3->value());
    c = QString::number(ui->dial_4->value());
    setTimeMotors(a, b, c);
    ui->lcdNumber_3->setStyleSheet(styleLCDnumberTimes);
    ui->lcdNumber_4->setStyleSheet(styleLCDnumberTimes);
    ui->lcdNumber_5->setStyleSheet(styleLCDnumberTimes);
}

void MainWindow::preSetTimeMotors(){
    //Estos son los tiempos de activacion que yo recomiendo
    QString a = "250", b = "100", c = "120";//a = adelante, b = atras, c = laterales
    int aI = a.toInt(), bI = b.toInt(), cI = c.toInt();
    setTimeMotors(a, b, c);
    ui->lcdNumber_3->display(aI);
    ui->dial_2->setValue(aI);
    ui->lcdNumber_4->display(bI);
    ui->dial_3->setValue(bI);
    ui->lcdNumber_5->display(cI);
    ui->dial_4->setValue(cI);
}

void MainWindow::on_pushButton_10_clicked()
{
    preSetTimeMotors();
    ui->lcdNumber_3->setStyleSheet(styleLCDnumberTimes);
    ui->lcdNumber_4->setStyleSheet(styleLCDnumberTimes);
    ui->lcdNumber_5->setStyleSheet(styleLCDnumberTimes);
}

