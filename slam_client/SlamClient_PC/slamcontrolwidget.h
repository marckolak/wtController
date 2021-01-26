#ifndef SLAMCONTROLWIDGET_H
#define SLAMCONTROLWIDGET_H

#include <QWidget>
#include <QUdpSocket>

namespace Ui {
class SlamControlWidget;
}

class SlamControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SlamControlWidget(QWidget *parent = nullptr);
    ~SlamControlWidget();

private:
    Ui::SlamControlWidget *ui;

    //! socket for outgoing communication
    QUdpSocket* socket;


public:
    /**
     * @brief setSocket set pointer to transmit socket
     */
    void setSocket(QUdpSocket* socket);


private slots:
    void onStartScanning(bool toggled);
};

#endif // SLAMCONTROLWIDGET_H
