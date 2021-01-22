#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

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

    //! socket for outgoing communication
    QUdpSocket *socket;

    //! socket for incoming communication
    QUdpSocket *rcvSocket;

    //! robot movement speed
    double speed = 0;

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void upKeyPressed(void);
    void upKeyReleased(void);

    void leftKeyPressed(void);
    void leftKeyReleased(void);

    void rightKeyPressed(void);
    void rightKeyReleased(void);

    void downKeyPressed(void);
    void downKeyReleased(void);



private slots:
    //!
    //! \brief onConnect connect to the robot
    //!
    //! Send a message containg the port, on which the app will listen for data.
    //!
    //! \param toggled send 'connect' command if true, else send 'disconnect'
    //!
    void onConnect(bool toggled);

    void onUpPressed(void);
    void onUpReleased(void);

    void onLeftPressed(void);
    void onLeftReleased(void);

    void onRightPressed(void);
    void onRightReleased(void);

    void onDownPressed(void);
    void onDownReleased(void);

    //!
    //! \brief onSliderValueChanged called when the speed slider value changes
    //! \param value current slider value
    //!
    void onSliderValueChanged(int value);

    //!
    //! \brief onScan sneds a command to the robot to either start or stop scanning
    //! \param toggled whether it is turned of or not
    //!
    void onScan(bool toggled);

    void onStatus();

    //!
    //! \brief readPendingData reads data coming to rcvSocket
    //!
    void readPendingData(void);
};
#endif // MAINWINDOW_H
