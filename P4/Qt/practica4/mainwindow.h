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
    //Definiciones styleShet de los botones
    QString preSetButton = "QPushButton {"
                           "   border-radius: 10px;"
                           "   border: 2px solid #555;"
                           "   background-color: #4CAF50;"
                           "   padding: 5px;"
                           "}"
                           "QPushButton:hover {"
                           "   background-color: #45a049;"  // Color al pasar el mouse
                           "}"
                           "QPushButton:pressed {"
                           "   background-color: #3e8e41;"  // Color al presionar
                           "}";
    QString preSetTimeButton = "QPushButton {"
                           "   border-radius: 10px;"
                           "   border: 2px solid #555;"
                           "   background-color: #e67e22;"
                           "   padding: 5px;"
                           "}"
                           "QPushButton:hover {"
                           "   background-color:  #ca6f1e;"  // Color al pasar el mouse
                           "}"
                           "QPushButton:pressed {"
                           "   background-color: #e67e22;"  // Color al presionar
                           "}";
    QString setReconectButton = "QPushButton {"
                               "   border-radius: 10px;"
                               "   border: 2px solid #555;"
                               "   background-color: #a93226;"
                               "   padding: 5px;"
                               "}"
                               "QPushButton:hover {"
                               "   background-color:  #c0392b;"  // Color al pasar el mouse
                               "}"
                               "QPushButton:pressed {"
                               "   background-color: #a93226;"  // Color al presionar
                               "}";
    QString setStyleMyComboBox = "QComboBox {"
                                "   border-radius: 10px;"
                                "   border: 1px solid #8e44ad;"
                                "   background: black;"
                                "   color: white;"
                                "}"
                                "QComboBox:hover {"
                                "   border: 1px solid #4CAF50;"
                                "}"
                                "QComboBox::drop-down {"
                                "   subcontrol-origin: padding;"
                                "   subcontrol-position: right center;"
                                "   width: 15px;"
                                "   border-left: 1px solid #f7dc6f;"
                                "}"
                                "QComboBox::down-arrow {"
                                ///"   image: none;"  // Desactiva la flecha predeterminada
                                "   width: 1;"
                                "   height: 1;"
                                "   border-left: 5px solid transparent;"
                                "   border-right: 5px solid transparent;"
                                "   border-top: 5px solid #f7dc6f;"  // Color amarillo (igual que la línea divisoria)
                                "}";
    QString setStyleDialServo = "QDial {"
                                "   background-color: #f0f0f0;"    // Fondo del dial
                                "   border-radius: 20px;"          // Bordes completamente redondeados (círculo)
                                "   border: 2px solid #aaa;"       // Borde
                                "}"
                                "QDial::handle {"
                                "   background: #4CAF50;"          // Color del indicador (agarre)
                                "   width: 12px;"                  // Ancho del indicador
                                "   height: 12px;"                 // Alto del indicador
                                "   border-radius: 6px;"           // Forma circular
                                "}";
    QString setStyleDialTime = "QDial {"
                                "   background-color: #2c3e50;"    // Fondo del dial
                                "   border-radius: 20px;"          // Bordes completamente redondeados (círculo)
                                "   border: 2px solid #aaa;"       // Borde
                                "}"
                                "QDial::handle {"
                                "   background: #4CAF50;"          // Color del indicador (agarre)
                                "   width: 12px;"                  // Ancho del indicador
                                "   height: 12px;"                 // Alto del indicador
                                "   border-radius: 6px;"           // Forma circular
                                "}";
    QString styleButtonDir = "QPushButton{"
                           "color: black;" //color de la letra en el boton
                           "background: #2ecc71;"
                           "border-radius: 35px;"
                           "border: 2px solid #01631f;"
                           "}"
                           "QPushButton:hover {"
                           "background-color: #1d8348;"  // Color al pasar el mouse
                           "}"
                           "QPushButton:pressed {"
                           "   background-color: #e67e22;"  // Color al presionar
                           "}";
    QString styleButtonStop = "QPushButton{"
                             "color: black;"
                             "background: #e74c3c;"
                             "border-radius: 25px;"
                             "border: 2px solid #641e16;"
                             "}"
                             "QPushButton:hover {"
                             "background-color: #c0392b;"  // Color al pasar el mouse
                             "}"
                              "QPushButton:pressed {"
                              "   background-color: #e67e22;"  // Color al presionar
                              "}";
    QString checkboxStyle = "QCheckBox {"
                            "   spacing: 5px;"                     // Espacio entre el indicador y el texto
                            "   color: #333333;"                   // Color del texto
                            "   font-size: 14px;"                  // Tamaño de fuente
                            "}"
                            "QCheckBox::indicator {"
                            "   width: 18px;"                      // Ancho del cuadro del checkbox
                            "   height: 18px;"                     // Alto del cuadro del checkbox
                            "   border: 2px solid #8e44ad;"        // Borde morado
                            "   border-radius: 4px;"               // Bordes ligeramente redondeados
                            "   background: white;"                // Fondo del cuadro (inactivo)
                            "}"
                            "QCheckBox::indicator:hover {"
                            "   border: 2px solid #4CAF50;"        // Borde verde al pasar el mouse
                            "}"
                            "QCheckBox::indicator:checked {"
                            "   background: #8e44ad;"              // Fondo morado cuando está marcado
                            "}"
                            "QCheckBox::indicator:disabled {"
                            "   border: 2px solid #cccccc;"        // Borde gris cuando está deshabilitado
                            "   background: #f0f0f0;"
                            "}";
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
    void preSetTimeMotors();
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
