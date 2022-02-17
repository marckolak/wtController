#include "slamcontrolwidget.h"
#include "ui_slamcontrolwidget.h"
#include "settings.h"



SlamControlWidget::SlamControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlamControlWidget)
{
    ui->setupUi(this);
}

SlamControlWidget::~SlamControlWidget()
{
    delete ui;
}

void SlamControlWidget::setSocket(QUdpSocket* socket)
{
    this->socket = socket;
}



void SlamControlWidget::onStartScanning(bool toggled)
{
    if(toggled)
    {
        QByteArray message = QString("{\"cmd\": \"scan\","
                                       "\"payload\": {"
                                            "\"action\": %1,"
                                            " \"speed\": %2,"
                                            " \"rate\":%3,"
                                            " \"send_scans\": %4}}")
                .arg("\"start\"").arg(2).arg(750).arg(ui->sendScansBox->isChecked())
                .toLocal8Bit();

        socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
        ui->scanButton->setText("Stop Scanning");
    }
    else
    {

        QByteArray message = QString("{\"cmd\": \"scan\", \"payload\": {\"action\": %1}}")
                .arg("\"stop\"")
                .toLocal8Bit();

        socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
        ui->scanButton->setText("Start Scanning");
    }

}

void SlamControlWidget::onStartCirCollection(bool toggled)
{
    if(toggled)
    {
        QByteArray message = QString("{\"cmd\": \"cir\","
                                       "\"payload\": {"
                                            "\"action\": %1,"
                                            " \"send_stats\": %2}}")
                .arg("\"start\"").arg(ui->sendCIRnoBox->isChecked())
                .toLocal8Bit();

        socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
        ui->cirButton->setText("Stop");
    }
    else
    {

        QByteArray message = QString("{\"cmd\": \"cir\", \"payload\": {\"action\": %1}}")
                .arg("\"stop\"")
                .toLocal8Bit();

        socket->writeDatagram(message, message.size(), QHostAddress(Settings::host), Settings::port);
        ui->cirButton->setText("Start");
    }

}

