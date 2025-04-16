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
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //MainWindow(QWidget *parent = nullptr);
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent *event) override;  // Detecta teclas presionadas
    //void keyReleaseEvent(QKeyEvent *event) override;  // Detecta teclas liberadas
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
    void on_pushButton_7_clicked();

    void on_pushButton_5_clicked();

    void setData(double temp, qint64 timestamp);
    void msgsUltra();
    void on_pushButton_8_pressed();

    void on_dial_2_sliderMoved(int position);

    void on_dial_2_sliderReleased();

    void on_dial_3_sliderMoved(int position);

    void on_dial_3_sliderReleased();

    void on_dial_4_sliderMoved(int position);

    void on_dial_4_sliderReleased();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void setTimeMotors(QString adelante, QString atras, QString laterales);
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
