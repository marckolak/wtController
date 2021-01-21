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

    QUdpSocket *socket;

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
    void onConnect(bool toggled);

    void onUpPressed(void);
    void onUpReleased(void);

    void onLeftPressed(void);
    void onLeftReleased(void);

    void onRightPressed(void);
    void onRightReleased(void);

    void onDownPressed(void);
    void onDownReleased(void);

    void onSliderValueChanged(int value);

    void onScan(bool toggled);
};
#endif // MAINWINDOW_H
