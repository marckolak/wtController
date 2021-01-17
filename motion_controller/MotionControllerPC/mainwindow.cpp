#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    ui->speedValueLabel->setText("0");
}

MainWindow::~MainWindow()
{

    socket->close();
    delete ui;
}


void MainWindow::onConnect(bool toggled)
{
    if(toggled)
    {

        socket->connectToHost("192.168.0.200", 4312);

        if(socket->waitForConnected(500))
        {
            qDebug() << "Connected!";
            ui->textEdit->setText("Connected to the controller.");
        }
        else
        {
            qDebug() << "Not connected!";
            ui->textEdit->setText("Can't connect to the controller.");

        }
    }
    else
    {
        socket->close();
        qDebug() << "Closed!";
        ui->textEdit->setText("Disconnected from the controller.");

    }


}

void MainWindow::onSliderValueChanged(int value)
{
    speed = value/100.0;
    qDebug() << speed;
    ui->speedValueLabel->setText(QString::number(speed));
}


void MainWindow::onUpPressed(void)
{
    ui->textEdit->setText(QString("Moving forward with speed: %1").arg(speed));
    socket->write(QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"forward\", \"speed\": %1, \"time\": 0 }}")
                  .arg(speed)
                  .toLocal8Bit());
}

void MainWindow::onUpReleased(void)
{
    ui->textEdit->setText(QString("Stopped"));
    socket->write(QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"stop\", \"speed\": %1, \"time\": 0 }}")
                  .arg(0)
                  .toLocal8Bit());
}

void MainWindow::onLeftPressed(void)
{
    ui->textEdit->setText(QString("Turning left with speed: %1").arg(speed));
    socket->write(QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"left\", \"speed\": %1, \"time\": 0 }}")
                  .arg(speed)
                  .toLocal8Bit());
}

void MainWindow::onLeftReleased(void)
{
    ui->textEdit->setText(QString("Stopped"));
    socket->write(QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"stop\", \"speed\": %1, \"time\": 0 }}")
                  .arg(0)
                  .toLocal8Bit());
}

void MainWindow::onRightPressed(void)
{
    ui->textEdit->setText(QString("Turning right with speed: %1").arg(speed));
    socket->write(QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"right\", \"speed\": %1, \"time\": 0 }}")
                  .arg(speed)
                  .toLocal8Bit());
}

void MainWindow::onRightReleased(void)
{
    ui->textEdit->setText(QString("Stopped"));
    socket->write(QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"stop\", \"speed\": %1, \"time\": 0 }}")
                  .arg(0)
                  .toLocal8Bit());
}

void MainWindow::onDownPressed(void)
{
    ui->textEdit->setText(QString("Moving backward with speed: %1").arg(speed));
    socket->write(QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"reverse\", \"speed\": %1, \"time\": 0 }}")
                  .arg(speed)
                  .toLocal8Bit());
}

void MainWindow::onDownReleased(void)
{
    ui->textEdit->setText(QString("Stopped"));
    socket->write(QString("{\"cmd\": \"move\", \"payload\": {\"direction\": \"stop\", \"speed\": %1, \"time\": 0 }}")
                  .arg(0)
                  .toLocal8Bit());
}
