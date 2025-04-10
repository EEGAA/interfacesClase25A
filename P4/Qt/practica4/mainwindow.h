#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QChartView>
#include <QLineSeries>
#include <QList>
#include <QChart>
#include <QValueAxis>
#include <QTimer>

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
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void sendHeartbeat();
    void connectWebSocket(const QUrl &url);


    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void loop();
    void moveMotor(QString direccion);
    void on_pushButton_6_clicked();

    void on_dial_sliderMoved(int position);
    void on_dial_sliderReleased();
    void setDial(int angulo);
    void sendJSON(QJsonObject objJSON);
private:
    Ui::MainWindow *ui;
    QWebSocket *m_webSocket;   // Puntero al WebSocket
    bool m_connected;
};
#endif // MAINWINDOW_H
