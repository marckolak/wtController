#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

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

    QTcpSocket *socket;

    double speed = 0;

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

};
#endif // MAINWINDOW_H
