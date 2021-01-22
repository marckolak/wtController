#include "mainwindow.h"
#include "settings.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QKeyEvent>
#include <QDateTime>
#include <QNetworkInterface>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // connect keyboard related signals and slots
    connect(this, &MainWindow::upKeyPressed, this, &MainWindow::onUpPressed);
    connect(this, &MainWindow::upKeyReleased, this, &MainWindow::onUpReleased);

    connect(this, &MainWindow::leftKeyPressed, this, &MainWindow::onLeftPressed);
    connect(this, &MainWindow::leftKeyReleased, this, &MainWindow::onLeftReleased);

    connect(this, &MainWindow::rightKeyPressed, this, &MainWindow::onRightPressed);
    connect(this, &MainWindow::rightKeyReleased, this, &MainWindow::onRightReleased);

    connect(this, &MainWindow::downKeyPressed, this, &MainWindow::onDownPressed);
    connect(this, &MainWindow::downKeyReleased, this, &MainWindow::onDownReleased);

    ui->setupUi(this);
    socket = new QUdpSocket(this);
    rcvSocket = new QUdpSocket(this);
    ui->speedValueLabel->setText("0");

    // connect socket signal with main window slot
    connect(rcvSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingData()));

    Settings::loadSettings();

    // grab all keyboard events
    this->grabKeyboard();
}

MainWindow::~MainWindow()
{
    this->releaseKeyboard();
    socket->close();
    delete ui;
}


void MainWindow::onConnect(bool toggled)
{
    if(toggled)
    {

        rcvSocket->bind( QHostAddress("192.168.0.45"), Settings::rcvPort);
        QByteArray message = QString("{\"cmd\": \"connect\", \"payload\": {\"port\": %1}}")
                .arg(Settings::rcvPort)
                .toLocal8Bit();
        const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
        QList<QHostAddress> list = QNetworkInterface::allAddresses();

        for(int nIter=0; nIter<list.count(); nIter++)

        {
            if(!list[nIter].isLoopback())
                if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
                    qDebug() << list[nIter].toString();

        }
        socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
        ui->statusButton->setEnabled(true);
    }
    else
    {

        QByteArray message = QString("{\"cmd\": \"disconnect\", \"payload\": {\"port\": %1}}")
                .arg(Settings::rcvPort)
                .toLocal8Bit();

        socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
        ui->statusButton->setEnabled(false);
    }

}

void MainWindow::onSliderValueChanged(int value)
{
    speed = value/100.0; // speed in 0-1 range
    qDebug() << speed;
    ui->speedValueLabel->setText(QString::number(speed));
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    bool autoRepeated = event->isAutoRepeat();

    if(!autoRepeated ) // run only if it's an original event
    {
        switch(event->key())
        {
        case Qt::Key_Up:
            emit upKeyPressed();
            break;
        case Qt::Key_Left:
            emit leftKeyPressed();
            break;
        case Qt::Key_Right:
            emit rightKeyPressed();
            break;
        case Qt::Key_Down:
            emit downKeyPressed();
            break;
        default:
            break;
        }
    }
}


void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    bool autoRepeated = event->isAutoRepeat();

    if(!autoRepeated)
    {
        switch(event->key()) // run only if it's an original event
        {
        case Qt::Key_Up:
            emit upKeyReleased();
            break;
        case Qt::Key_Left:
            emit leftKeyReleased();
            break;
        case Qt::Key_Right:
            emit rightKeyReleased();
            break;
        case Qt::Key_Down:
            emit downKeyReleased();
            break;
        default:
            break;

        }
    }

}


void MainWindow::onUpPressed(void)
{
    //    ui->plainTextEdit->appendPlainText(QString("Moving forward with speed: %1").arg(speed));
    QByteArray message = QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"forward\", \"speed\": %1, \"time\": 0 }}")
            .arg(speed)
            .toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
}

void MainWindow::onUpReleased(void)
{
    //    ui->plainTextEdit->appendPlainText(QString("Stopped"));
    QByteArray message = QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"stop\", \"speed\": %1, \"time\": 0 }}")
            .arg(0)
            .toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
}

void MainWindow::onLeftPressed(void)
{
    //    ui->plainTextEdit->appendPlainText(QString("Turning left with speed: %1").arg(speed));
    QByteArray message = QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"left\", \"speed\": %1, \"time\": 0 }}")
            .arg(speed)
            .toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
}

void MainWindow::onLeftReleased(void)
{
    //    ui->plainTextEdit->appendPlainText(QString("Stopped"));
    QByteArray message = QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"stop\", \"speed\": %1, \"time\": 0 }}")
            .arg(0)
            .toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
}

void MainWindow::onRightPressed(void)
{
    //    ui->plainTextEdit->appendPlainText(QString("Turning right with speed: %1").arg(speed));
    QByteArray message = QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"right\", \"speed\": %1, \"time\": 0 }}")
            .arg(speed)
            .toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);

}

void MainWindow::onRightReleased(void)
{
    //    ui->plainTextEdit->appendPlainText(QString("Stopped"));
    QByteArray message = QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"stop\", \"speed\": %1, \"time\": 0 }}")
            .arg(0)
            .toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);

}

void MainWindow::onDownPressed(void)
{
    //    ui->plainTextEdit->appendPlainText(QString("Moving backward with speed: %1").arg(speed));
    QByteArray message = QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"reverse\", \"speed\": %1, \"time\": 0 }}")
            .arg(speed)
            .toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);

}

void MainWindow::onDownReleased(void)
{
    //    ui->plainTextEdit->appendPlainText(QString("Stopped"));
    QByteArray message = QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"stop\", \"speed\": %1, \"time\": 0 }}")
            .arg(0)
            .toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);

}


void MainWindow::onScan(bool toggled)
{
    if(toggled)
    {
        //        ui->plainTextEdit->appendPlainText(QString("Start Scanning"));
        QByteArray message = QString("{\"cmd\": \"scan\", \"payload\": {\"action\": \"start\", \"speed\": 5, \"rate\": 1 }}")
                .toLocal8Bit();
        socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);

    }
    else
    {
        //        ui->plainTextEdit->appendPlainText(QString("Stop Scanning"));
        QByteArray message = QString("{\"cmd\": \"scan\", \"payload\": {\"action\": \"stop\", \"speed\": 0, \"rate\": 0 }}")
                .toLocal8Bit();
        socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);

    }


}

void MainWindow::onStatus()
{
    QByteArray message = QString("{\"cmd\": \"status\", \"payload\": {}}")
            .toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);

}


void MainWindow::readPendingData()
{
    // read incoming packet
    QByteArray rPacket;
    rPacket.resize(rcvSocket->pendingDatagramSize());
    rcvSocket->readDatagram(rPacket.data(), rcvSocket->pendingDatagramSize());

    QDateTime now = QDateTime::currentDateTime();
    QString dateStr = now.toString("hh:mm:ss.z");

    QString message = QString::fromUtf8(rPacket);
    ui->plainTextEdit->appendPlainText(dateStr+"\t"+message);

}
