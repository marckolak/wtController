#include "plotwidget.h"
#include "ui_plotwidget.h"

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
