#ifndef MOTIONCONTROLWIDGET_H
#define MOTIONCONTROLWIDGET_H

#include <QWidget>
#include <QUdpSocket>


namespace Ui {
class MotionControlWidget;
}

/**
 * @brief The MotionControlWidget class implements a widget used to remotely control the robot movement.
 */
class MotionControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MotionControlWidget(QWidget *parent = nullptr);
    ~MotionControlWidget();

private:
    Ui::MotionControlWidget *ui;

    //! socket for outgoing communication
    QUdpSocket* socket;

    /** robot movement speed */
    double speed = 0;

    const QString moveTemplate = "{\"cmd\": \"move\","
                                    "\"payload\": {"
                                            "\"direction\": \"%1\", "
                                            "\"speed\": %2, "
                                            "\"time\": 0 }}";

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

public:

    /**
     * @brief setSocket set pointer to transmit socket
     */
    void setSocket(QUdpSocket* socket);

signals:
    /** Emmited when the UP arrow key is pressed */
    void upKeyPressed(void);

    /** Emmited when the LEFT arrow key is pressed */
    void leftKeyPressed(void);

    /** Emmited when the RIGHT arrow key is pressed */
    void rightKeyPressed(void);

    /** Emmited when the DOWN arrow key is pressed */
    void downKeyPressed(void);

    /** Emmited when any of the arrow keys is released */
    void arrowKeyReleased(void);

private slots:
    /** Called when the UP arrow button is pressed */
    void onUpPressed(void);

    /** Called when the LEFT arrow button is pressed */
    void onLeftPressed(void);

    /** Called when the RIGHT arrow button is pressed */
    void onRightPressed(void);

    /** Called when the DOWN arrow button is pressed */
    void onDownPressed(void);

    /** Called when any of the arrow buttons is released */
    void onArrowKeyReleased(void);

    /**
     * @brief onSliderValueChanged called when the speed slider value changes
     * @param value current slider value
     */
    void onSliderValueChanged(int value);

    /**
     * @brief onKeyboardChecked called when the 'Keyboard control' checkbox is checked
     * @param value scheckbox state
     */
    void onKeyboardChecked(int value);
};

#endif // MOTIONCONTROLWIDGET_H
