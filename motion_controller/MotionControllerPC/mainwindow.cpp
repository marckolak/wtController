#include "mainwindow.h"
#include "settings.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QKeyEvent>

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
    socket = new QTcpSocket(this);
    ui->speedValueLabel->setText("0");

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
        socket->connectToHost(Settings::host, Settings::port);

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
