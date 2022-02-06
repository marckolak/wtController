#include "motioncontrolwidget.h"
#include "ui_motioncontrolwidget.h"
#include "settings.h"
#include <QKeyEvent>


MotionControlWidget::MotionControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionControlWidget)
{
    ui->setupUi(this);

    // connect keyboard related signals and slots
    connect(this, &MotionControlWidget::upKeyPressed, this, &MotionControlWidget::onUpPressed);
    connect(this, &MotionControlWidget::leftKeyPressed, this, &MotionControlWidget::onLeftPressed);
    connect(this, &MotionControlWidget::rightKeyPressed, this, &MotionControlWidget::onRightPressed);
    connect(this, &MotionControlWidget::downKeyPressed, this, &MotionControlWidget::onDownPressed);

    connect(this, &MotionControlWidget::arrowKeyReleased, this, &MotionControlWidget::onArrowKeyReleased);

}

MotionControlWidget::~MotionControlWidget()
{
    delete ui;
}


void MotionControlWidget::keyPressEvent(QKeyEvent *event)
{
    bool autoRepeated = event->isAutoRepeat();

    if(!autoRepeated && (this->keyboardGrabber() == this)) // run only if it's an original event
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


void MotionControlWidget::keyReleaseEvent(QKeyEvent *event)
{
    bool autoRepeated = event->isAutoRepeat();

    if(!autoRepeated && (this->keyboardGrabber() == this))
    {
        switch(event->key()) // run only if it's an original event
        {
        case Qt::Key_Up:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Down:
            emit arrowKeyReleased();
            break;
        default:
            break;

        }
    }

}


void MotionControlWidget::setSocket(QUdpSocket* socket)
{
    this->socket = socket;
}


void MotionControlWidget::onUpPressed(void)
{
    QByteArray message = this->moveTemplate.arg("forward").arg(speed).toLocal8Bit();
    this->socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
    qDebug()<<message;
}

void MotionControlWidget::onLeftPressed(void)
{
    QByteArray message = this->moveTemplate.arg("left").arg(speed).toLocal8Bit();
    this->socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
    qDebug()<<message;
}

void MotionControlWidget::onRightPressed(void)
{
    QByteArray message = this->moveTemplate.arg("right").arg(speed).toLocal8Bit();
    this->socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
    qDebug()<<message;
}

void MotionControlWidget::onDownPressed(void)
{
    QByteArray message = this->moveTemplate.arg("reverse").arg(speed).toLocal8Bit();
    this->socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
    qDebug()<<message;
}

void MotionControlWidget::onArrowKeyReleased(void)
{
    QByteArray message = this->moveTemplate.arg("stop").arg(speed).toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
    qDebug()<<message;

}

void MotionControlWidget::onSliderValueChanged(int value)
{
    speed = value/100.0; // speed in 0-1 range
    qDebug() << speed;
    ui->speedValueLabel->setText(QString::number(speed));
}


void MotionControlWidget::onKeyboardChecked(int value)
{
    if(value==2)
    {
        this->grabKeyboard();
    }
    else
    {
        this->releaseKeyboard();
    }

}
