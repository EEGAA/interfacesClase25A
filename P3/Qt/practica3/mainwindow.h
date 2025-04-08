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
    void on_dial_sliderMoved(int position);

    void on_dial_sliderReleased();

    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void sendHeartbeat();
    void connectWebSocket(const QUrl &url);

    void loop();
    void setData(double temp, qint64 timestamp);
    void getDistancia();
    void setDial(int angulo);
    void on_pushButton_pressed();

    void auxiliar();
private:
    Ui::MainWindow *ui;
    QWebSocket *m_webSocket;   // Puntero al WebSocket
    bool m_connected;

    QChart *chartTemp;
    QLineSeries *seriesTemp;
    QValueAxis *axisXTemp;
    QValueAxis *axisYTemp;
    QList<QPointF> dataPointsTemp;
    qint64 startTime;
};
#endif // MAINWINDOW_H
