#ifndef PATHPLANNINGWIDGET_H
#define PATHPLANNINGWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QUdpSocket>

namespace Ui {
class PathPlanningWidget;
}

class PathSegment{

public:
    PathSegment();

    PathSegment(QString direction, float speed, float time);

    QString toJson();

private:

    QString jsonTemplate =  "{\"direction\": \"%1\", "
                            "\"speed\": %2, "
                            "\"time\": %3}";

    QString direction;

    float speed;

    float time;




};



class PathPlanningWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PathPlanningWidget(QWidget *parent = nullptr);
    ~PathPlanningWidget();


    void setSocket(QUdpSocket *socket);

private:
    Ui::PathPlanningWidget *ui;

    QUdpSocket *socket;

    std::vector<PathSegment> segments;


    // List related
    QString listTemplate = "%1; %2 \%; %3 s";

    QListWidget *segmentList;

    std::vector<QListWidgetItem*> items;


    // Input widgets
    QDoubleSpinBox *speedSpin;
    QDoubleSpinBox *timeSpin;
    QComboBox *dirCombo;




private slots:
    void onAddSegment();

    void onExecutePath();

    void onLoadFile();
};



#endif // PATHPLANNINGWIDGET_H
