#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"

#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // get pointers to specific widgets
    motionControl = ui->motionWidget;
    communications = ui->communicationsWidget;

    // load settings
    Settings::loadSettings();


    //--------- Set up network communications ---------//

    // create sockets
    socket = new QUdpSocket(this);
    rcvSocket = new QUdpSocket(this);

    // bind receive socket
    rcvSocket->bind( QHostAddress("192.168.0.45"), Settings::rcvPort);

    // connect rcvSocket signal with main window slot
    connect(rcvSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingData()));

    communications->setSocket(this->socket);
    motionControl->setSocket(this->socket);
}

MainWindow::~MainWindow()
{
    socket->close();
    rcvSocket->close();
    delete ui;
}


QUdpSocket* MainWindow::getSocket(void)
{
    return socket;
}



void MainWindow::readPendingData()
{
    // read incoming packet
    QByteArray rPacket;
    rPacket.resize(rcvSocket->pendingDatagramSize());
    rcvSocket->readDatagram(rPacket.data(), rcvSocket->pendingDatagramSize());
    QJsonObject message = QJsonDocument::fromJson(rPacket.data()).object();

    QString command = "";
    qDebug()<<rPacket.data();
    // get message type
    if (message.contains("cmd") && message["cmd"].isString())
        command = message["cmd"].toString();

    // pass the message to apropriate widget
    if(command == "status")
    {
        ;
    }else if(command == "scan")
    {
        ;
    }else
    {
        ;
    }

}

