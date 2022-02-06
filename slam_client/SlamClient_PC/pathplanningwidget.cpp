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

    // set pointers to ui elements
    segmentList = ui->motionList;
    dirCombo = ui->dirBox;
    speedSpin = ui->speedSpinBox;
    timeSpin = ui->timeSpinBox;
    waitSpin = ui->waitSpinBox;
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
    // message template
    QString message = QString("{\"cmd\": \"planned_path\","
                              "\"payload\": {"
                              "\"stop_time\": %1, "
                              "\"path\": [ ").arg(waitSpin->value());

    // append path segment jsons
    if(segments.size() >0)
    {
        message += segments[0].toJson();
        for(unsigned int i=1; i<segments.size(); i++)
        {
            message += ",";
            message += segments[i].toJson();
        }

    }
    message += "]}}";

    // send to the robot
    socket->writeDatagram(message.toLocal8Bit(), message.size(), QHostAddress(Settings::host), Settings::port);

    qDebug() << message;

}


void PathPlanningWidget::onLoadFile()
{
    // get path to file with defined robot path
    QString filepath = QFileDialog::getOpenFileName(this, tr("Path File"),
                                                    "",
                                                    tr("Path File(*.csv)"));

    // display filename
    ui->lineEdit->setText(filepath);

    //read file
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return;
    }

    // read line by line
    while (!file.atEnd()) {
        QString line = file.readLine().replace("\n", "");
        QStringList wordlist = line.split(",");

        // add segments to vector and list
        segments.push_back(PathSegment(wordlist.at(0), wordlist.at(1).toDouble(), wordlist.at(2).toDouble()));

        this->items.push_back(new QListWidgetItem(this->listTemplate.arg(wordlist.at(0))
                                                  .arg(wordlist.at(1))
                                                  .arg(wordlist.at(2))));

        segmentList->addItem(items[items.size()-1]);

    }

}


void  PathPlanningWidget::onClear()
{
    // clear vector and list of all segments
    segments.clear();
    segmentList->clear();

}
