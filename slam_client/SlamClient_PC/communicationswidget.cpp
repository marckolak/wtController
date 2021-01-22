#include "communicationswidget.h"
#include "ui_communicationswidget.h"

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
