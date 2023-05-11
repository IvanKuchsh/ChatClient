
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void PrintMessage(QString message);
    void Connect();
    void SendMessage();
    void SendToServer(QString message);
    void closeEvent(QCloseEvent *event);
private slots:
    void on_connectButton_clicked();
    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();
    void on_messageInput_returnPressed();
    void on_send_clicked();
    void on_nameInput_returnPressed();

private:
    Ui::MainWindow* ui;
    quint16         nextBlockSize_;
    bool            connectedToHost_;
    QTcpSocket*     socket_;
    QString         name_;
};

#endif // MAINWINDOW_H
