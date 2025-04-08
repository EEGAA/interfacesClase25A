#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLayout>
#include <QChartView>
#include <QLineSeries>
#include <QList>
#include <QChart>
#include <QChartView>
#include <QValueAxis>

#include <QDateTime>
#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>

#include <QtNetwork>

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

private slots:
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
    void loop();
    void setData(double temp, qint64 timestamp);
    void on_dial_sliderMoved(int position);

    void on_dial_sliderReleased();

    float getDistancia();
    void on_pushButton_pressed();

    void on_pushButton_2_pressed();

    void setDial(int angulo);
private:
    Ui::MainWindow *ui;

    QChart *chartTemp;
    QLineSeries *seriesTemp;
    QValueAxis *axisXTemp;
    QValueAxis *axisYTemp;

    QList<QPointF> dataPointsTemp;
    qint64 startTime;
};
#endif // MAINWINDOW_H
