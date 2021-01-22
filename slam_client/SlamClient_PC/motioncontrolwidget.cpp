#include "motioncontrolwidget.h"
#include "ui_motioncontrolwidget.h"

MotionControlWidget::MotionControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionControlWidget)
{
    ui->setupUi(this);
}

MotionControlWidget::~MotionControlWidget()
{
    delete ui;
}
