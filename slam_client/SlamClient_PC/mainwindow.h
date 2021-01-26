#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

#include "motioncontrolwidget.h"
#include "communicationswidget.h"
#include "slamcontrolwidget.h"
#include "plotwidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    MotionControlWidget *motionControl;

    CommunicationsWidget *communications;

    SlamControlWidget *slamControl;

    PlotWidget *visualization;

    //! socket for outgoing communication
    QUdpSocket *socket;

    //! socket for incoming communication
    QUdpSocket *rcvSocket;



public:
    QUdpSocket* getSocket(void);


private slots:

    void readPendingData(void);


};
#endif // MAINWINDOW_H
