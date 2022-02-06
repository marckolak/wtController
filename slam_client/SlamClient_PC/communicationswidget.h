#ifndef COMMUNICATIONSWIDGET_H
#define COMMUNICATIONSWIDGET_H

#include <QWidget>
#include <QUdpSocket>

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

    //! socket for outgoing communication
    QUdpSocket* socket;

public:

    /**
     * @brief setSocket set pointer to transmit socket
     */
    void setSocket(QUdpSocket* socket);


    /**
     * @brief processMessage process message received from the Pi controller
     * @param message json with the message
     * @param message_type message type
     */
    void processMessage(QJsonObject message, QString message_type);

private slots:
    /**
     * @brief onConnect onConnect connect to the robot
     *
     * Send a message containg the port, on which the app will listen for data
     *
     * @param toggled toggled send 'connect' command if true, else send 'disconnect'
     */
    void onConnect(bool toggled);

    /**
     * @brief onStatus called when get status button is clicked
     */
    void onGetStatus(void);
};

#endif // COMMUNICATIONSWIDGET_H
