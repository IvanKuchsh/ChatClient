#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectedToHost_ = false;
    nextBlockSize_ = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Connect() {
    if (!connectedToHost_)
    {
        QString name = ui->nameInput->text().replace(" ", "");
        for (int i = 0; i < name.size(); ++i)
        {
            if (!(name[i].isLetter()))
            {
                ui->nameInput->setText("Введите корректное имя!");
                return;
            }
        }
        if (name.isEmpty())
        {
            ui->nameInput->setText("Введите корректное имя!");
            return;
        }
        socket_ = new QTcpSocket();
        connect(socket_, SIGNAL(connected()), this, SLOT(socketConnected()));
        connect(socket_, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
        connect(socket_, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
        socket_->connectToHost("127.0.0.1", 8001);
    }
    else
    {
        SendToServer("<font color=\"Red\">" + name_ + " отключился(ась) от чата.</font>");
        socket_->disconnectFromHost();
    }
}
void MainWindow::on_nameInput_returnPressed()
{
    Connect();
}
void MainWindow::on_connectButton_clicked()
{
    Connect();
}

void MainWindow::SendToServer(QString message) {
    QByteArray arrBlock;
    arrBlock.clear();
    QDataStream out(&arrBlock, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_6_5);
    out << quint16(0);
    out << message;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    socket_->write(arrBlock);
}

void MainWindow::socketConnected()
{
    name_ = ui->nameInput->text();
    PrintMessage("<font color=\"Green\">Вы подключились к чату.</font>");
    SendToServer("<font color=\"Purple\">" + name_.toUtf8() + " подключился(ась) к чату.</font>");
    ui->connectButton->setText("Отключиться");
    ui->connectButton->setStyleSheet(ui->connectButton->styleSheet() + "QPushButton:hover{color:red; border: 1px solid red}");
    connectedToHost_ = true;
}

void MainWindow::socketDisconnected()
{
    PrintMessage("<font color=\"Red\">Вы отключились от чата.</font>");
    ui->connectButton->setText("Подключиться");
    ui->connectButton->setStyleSheet(ui->connectButton->styleSheet() + "QPushButton:hover{color:#2fff00; border: 1px solid #2fff00;}");
    connectedToHost_ = false;
}

void MainWindow::socketReadyRead()
{
    QDataStream in(socket_);
    in.setVersion(QDataStream::Qt_6_5);

    while (true)
    {
        if (nextBlockSize_ == 0)
        {
            if (socket_->bytesAvailable() < static_cast<int>(sizeof(quint16)))
            {
                break;
            }
            in >> nextBlockSize_;
        }
        if (socket_->bytesAvailable() < nextBlockSize_)
        {
            break;
        }
        QString str;
        in >> str;
        ui->chatDisplay->append(str);

        nextBlockSize_ = 0;
        break;
    }
}

void MainWindow::PrintMessage(QString message)
{
    ui->chatDisplay->append(message);
}
QString DeleteSpaces(const QString message) {
    int i = 0;
    for (; i < message.size(); ++i) {
        if (message[i] != ' ') {
            break;
        }
    }
    return QString(message.begin() + i);
}
void MainWindow::SendMessage() {
    if (connectedToHost_) {
        QString message = DeleteSpaces(ui->messageInput->text());
        if (message.size() != 0) {
            ui->chatDisplay->append("<font style=\"color:orange;\">Ваше сообщение""</font>: " + message);
            SendToServer("<font color=\"blue\">" + name_ + "</font>: " + message);
            ui->messageInput->clear();
        } else {
            ui->messageInput->clear();
        }
    }

}
void MainWindow::on_send_clicked()
{
    SendMessage();
}
void MainWindow::on_messageInput_returnPressed()
{
    SendMessage();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    SendToServer("<font color=\"Red\">" + name_ + " отключился(ась) от чата.</font>");
    socket_->disconnectFromHost();
}
