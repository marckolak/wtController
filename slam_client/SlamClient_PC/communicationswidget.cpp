#include "communicationswidget.h"
#include "ui_communicationswidget.h"
#include "settings.h"
#include <QJsonObject>


CommunicationsWidget::CommunicationsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommunicationsWidget)
{
    ui->setupUi(this);
}

CommunicationsWidget::~CommunicationsWidget()
{
    delete ui;
}


void CommunicationsWidget::setSocket(QUdpSocket* socket)
{
    this->socket = socket;
}

void CommunicationsWidget::onConnect(bool toggled)
{

    if(toggled)
    {
        QByteArray message = QString("{\"cmd\": \"connect\", \"payload\": {\"port\": %1}}")
                .arg(Settings::rcvPort)
                .toLocal8Bit();

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


void CommunicationsWidget::onGetStatus()
{
    QByteArray message = QString("{\"cmd\": \"status\", \"payload\": {}}")
            .toLocal8Bit();
    socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);

}


void CommunicationsWidget::processMessage(QJsonObject message, QString message_type)
{
    QJsonObject payload;
    // get message type
    if (message.contains("payload") && message["payload"].isObject())
        payload = message["payload"].toObject();

    if(message_type == "direct_print")
    {
        if (payload.contains("text") && payload["text"].isString())
        {
            QString text = payload["text"].toString();
            ui->plainTextEdit->appendPlainText(text);
        }

    }
    else if(message_type == "status")
    {
        QString left_motor, right_motor, scanner, start_time;

        if (payload.contains("left_motor") && payload["left_motor"].isString())
            left_motor = payload["left_motor"].toString();
        if (payload.contains("right_motor") && payload["right_motor"].isString())
            right_motor = payload["right_motor"].toString();
        if (payload.contains("scanner") && payload["scanner"].isString())
            scanner = payload["scanner"].toString();
        if (payload.contains("start_time") && payload["start_time"].isString())
            start_time = payload["start_time"].toString();

        QString statusString = QString("Status: \n"
                               "left motor: %1 \n"
                               "right motor: %2 \n"
                               "scanner: %3 \n"
                               "start_time: %4").arg(left_motor).arg(right_motor).arg(scanner).arg(start_time);

        ui->plainTextEdit->appendPlainText(statusString);
    }

}
