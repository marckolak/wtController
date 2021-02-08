#include "pathplanningwidget.h"
#include "ui_pathplanningwidget.h"
#include <QDebug>
#include "settings.h"
#include <QFileDialog>

//*********************
// Path Segment methods
//*********************

PathSegment::PathSegment()
{

}

PathSegment::PathSegment(QString direction, float speed, float time)
{
    this->direction = direction;
    this->speed = speed;
    this->time = time;
}


QString PathSegment::toJson()
{
    return this->jsonTemplate.arg( this->direction ).arg(this->speed).arg(this->time);
}



//*********************
// Path Planing Widget methods
//*********************

PathPlanningWidget::PathPlanningWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PathPlanningWidget)
{
    ui->setupUi(this);

    segmentList = ui->motionList;
    dirCombo = ui->dirBox;
    speedSpin = ui->speedSpinBox;
    timeSpin = ui->timeSpinBox;
}

PathPlanningWidget::~PathPlanningWidget()
{
    delete ui;
}

void PathPlanningWidget::setSocket(QUdpSocket* socket)
{
    this->socket = socket;
}


void PathPlanningWidget::onAddSegment()
{
    segments.push_back(PathSegment(dirCombo->currentText(), speedSpin->value(), timeSpin->value()));

    this->items.push_back(new QListWidgetItem(this->listTemplate.arg(dirCombo->currentText()).arg(speedSpin->value()).arg(timeSpin->value())));
    segmentList->addItem(items[items.size()-1]);


    qDebug() << items.size();

}

void PathPlanningWidget::onExecutePath()
{

    QString message = QString("{\"cmd\": \"planned_path\","
                              "\"payload\": {"
                              "\"stop_time\": %1, "
                              "\"path\": [ ").arg(2);

    if(segments.size() >0)
    {
        message += segments[0].toJson();
        for(unsigned int i=0; i<segments.size(); i++)
        {
            message += ",";
            message += segments[i].toJson();
        }

    }
    message += "]}}";

    qDebug() << message;

    socket->writeDatagram(message.toLocal8Bit(), message.size(), QHostAddress(Settings::host), Settings::port);

}


void PathPlanningWidget::onLoadFile()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("OutputFile"),
                                                    "",
                                                    tr("Path File(*.csv)"));

    ui->lineEdit->setText(filepath);


    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return;
    }

    while (!file.atEnd()) {
        QString line = file.readLine();
        QStringList wordlist = line.split(",");


        segments.push_back(PathSegment(wordlist.at(0), wordlist.at(1).toDouble(), wordlist.at(2).toDouble()));

        this->items.push_back(new QListWidgetItem(this->listTemplate.arg(wordlist.at(0))
                                                  .arg(wordlist.at(1))
                                                  .arg(wordlist.at(2))));

        segmentList->addItem(items[items.size()-1]);


        qDebug() << wordlist;

    }



}

