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

/**
 * @brief The PathSegment class is a data holder for a programmed path segment containing speed and time
 */
class PathSegment{

public:
    /** Default constructor*/
    PathSegment();

    /**
     * @brief PathSegment constructor, which allows to set direction, speed and time values
     * @param direction direction in which a robot will move e.g. "forward", "left"
     * @param speed speed in 0-1 range
     * @param time time, for which the robot will go with the set speed
     */
    PathSegment(QString direction, float speed, float time);

    /**
     * @brief toJson convert a PathSegment object to json
     * @return json in format {"direction": direction, "speed": speed, "time": time}
     */
    QString toJson();

private:
    /** Template for json being a part of the command*/
    QString jsonTemplate =  "{\"direction\": \"%1\", "
                            "\"speed\": %2, "
                            "\"time\": %3}";
    /** Direction of movement*/
    QString direction;

    /** Speed in 0-1 range*/
    float speed;

    /** Time, for which the robot will go with the set speed*/
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
    QDoubleSpinBox *waitSpin;

    QComboBox *dirCombo;




private slots:
    void onAddSegment();

    void onExecutePath();

    void onLoadFile();

    void onClear();
};



#endif // PATHPLANNINGWIDGET_H
