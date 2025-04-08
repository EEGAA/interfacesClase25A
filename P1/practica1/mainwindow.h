#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include "QDebug"
#include <math.h>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    //Hoja de estilo de Qt - qss, similar a css de html
    QString activado = "QPushButton{"
                       "background: #2ecc71;"
                       "border-radius: 35px;"
                       "border: 2px solid #01631f;"
                       "}";
    QString desactivado = "QPushButton{"
                          "background: #bdc3c7;"
                          "border-radius: 35px;"
                          "border: 2px solid #7f8c8d;"
                          "}";
    QString btnGeneral = "QPushButton{"
                         "background: #717372"
                         "}";
public slots:
    QString conexionWeb(QString url, QString mensaje, QString nombreRed){
        QString respuesta = "error 1";

        // Paso #1: Verificar que la red sea una red funcional.
        QNetworkInterface redConectada = QNetworkInterface::interfaceFromName(nombreRed);
        QList<QNetworkAddressEntry> lista = redConectada.addressEntries();

        // Paso #2: Verificar que la red esté activa.
        if (!lista.empty()) {
            QNetworkAddressEntry IP = lista.first();
            qDebug() << "Red activa: " << IP.ip();

            // Crear el mensaje HTML/HTTP
            QByteArray longitudMensaje = QByteArray::number(mensaje.size());
            QNetworkRequest solicitud;
            QNetworkAccessManager *clienteWeb = new QNetworkAccessManager(this); // Manejo de memoria automático
            QUrl servidor(url.toUtf8().constData());

            // Paso #3: Verificar que la URL sea válida
            if (servidor.isValid()) {
                qDebug() << "Servidor válido";

                // Paso #4: Formar el mensaje HTTP
                solicitud.setUrl(servidor);
                solicitud.setRawHeader(QByteArray("User-Agent"), QByteArray("bot"));
                solicitud.setRawHeader(QByteArray("Connection"), QByteArray("close"));
                solicitud.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                solicitud.setHeader(QNetworkRequest::ContentLengthHeader, longitudMensaje);

                // Paso #5: Realizar la conexión
                QNetworkReply *conexionServidor = clienteWeb->post(solicitud, mensaje.toLatin1());

                // Paso #6: Esperar a que el servidor responda
                QEventLoop funcionLoop;
                QObject::connect(conexionServidor, &QNetworkReply::finished, &funcionLoop, &QEventLoop::quit);
                funcionLoop.exec();

                // Verificar errores en la respuesta
                if (conexionServidor->error() == QNetworkReply::NoError) {
                    // Paso #7: Leer la respuesta del servidor
                    QByteArray datosWeb = conexionServidor->readAll();
                    respuesta = QString(datosWeb);
                } else {
                    respuesta = "Error en la conexión: " + conexionServidor->errorString();
                }

                conexionServidor->deleteLater();
            } else {
                respuesta = "error 3";
            }
        } else {
            respuesta = "Error 2";
        }

        return respuesta;
    }
private slots:
    void on_pushButton_clicked(bool checked);

    void on_pushButton_2_clicked(bool checked);

    void on_pushButton_3_clicked(bool checked);

    void on_pushButton_4_clicked(bool checked);

    void on_pushButton_5_clicked(bool checked);

    void on_dial_sliderMoved(int position);

    void on_dial_sliderReleased();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void _5BITS(int decimal);

    void setStateLED(int LED, int bnd);
    void setCerosLED();
    void on_pushButton_8_clicked();

    void printStateButton(int LED, int bnd);
private:
    Ui::MainWindow *ui;
};



class LEDsControl{
private:
    QString led[5], aux, IP_, red_;
    int state[5];
public:
    LEDsControl(int a, int b, int c, int d, int e){
        this->led[0] = QString::number(a);
        this->led[1] = QString::number(b);
        this->led[2] = QString::number(c);
        this->led[3] = QString::number(d);
        this->led[4] = QString::number(e);
        /*this->IP_ = myIP;
        this->red_ = myRed;*/
        for(int i=0; i<5; i++)
            state[i] = 0;
    }
    QString stateLED(int LED, int bnd){
        if(state[LED] != bnd)
            state[LED] = bnd;
        QString bnd_ = QString::number(bnd);
        aux = "{\"dispositivo\":\"foco\", \"pin\":" + led[LED] + ",\"estado\":" + bnd_ + "}";
        return aux;
    }
    int getState(int led){
        return state[led];
    }
    int getStates(){
        int aux = 0, i;
        for(i=0; i<5; i++)
            aux = aux * 10 + state[i];
        return aux;
    }
    int getDecimal(){
        int aux = 0,  i;
        for(i=0; i<5; i++)
            if(state[4-i]==1)
                aux = aux + pow(2, i);
        return  aux;
    }
};

#endif // MAINWINDOW_H
