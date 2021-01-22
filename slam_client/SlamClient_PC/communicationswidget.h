#ifndef COMMUNICATIONSWIDGET_H
#define COMMUNICATIONSWIDGET_H

#include <QWidget>

namespace Ui {
class CommunicationsWidget;
}

class CommunicationsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommunicationsWidget(QWidget *parent = nullptr);
    ~CommunicationsWidget();

private:
    Ui::CommunicationsWidget *ui;
};

#endif // COMMUNICATIONSWIDGET_H
