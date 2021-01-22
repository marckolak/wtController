#ifndef MOTIONCONTROLWIDGET_H
#define MOTIONCONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class MotionControlWidget;
}

class MotionControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MotionControlWidget(QWidget *parent = nullptr);
    ~MotionControlWidget();

private:
    Ui::MotionControlWidget *ui;
};

#endif // MOTIONCONTROLWIDGET_H
