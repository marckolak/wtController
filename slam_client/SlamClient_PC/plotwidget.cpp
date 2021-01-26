#include "plotwidget.h"
#include "ui_plotwidget.h"
#include <QJsonObject>

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget)
{
    ui->setupUi(this);

    renderArea = ui->renderWidget;
}

PlotWidget::~PlotWidget()
{
    delete ui;
}


void PlotWidget::onZoomIn()
{
    renderArea -> zoomIn();

}

void PlotWidget::onZoomOut()
{

    renderArea -> zoomOut();
}



void PlotWidget::processMessage(QJsonObject message, QString message_type)
{
    QJsonObject payload;
    // get message type
    if (message.contains("payload") && message["payload"].isObject())
        payload = message["payload"].toObject();

    if(message_type == "scan")
    {
        renderArea->newScan(payload);
    }

}
