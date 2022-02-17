#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include "renderarea.h"

namespace Ui {
class PlotWidget;
}

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = nullptr);
    ~PlotWidget();


    /**
     * @brief processMessage process message received from the Pi controller
     * @param message json with the message
     * @param message_type message type
     */
    void processMessage(QJsonObject message, QString message_type);

private:
    Ui::PlotWidget *ui;

    /** RenderArea Widget, where the map is drawn*/
    RenderArea *renderArea;

private slots:
    /**
     * @brief onZoomIn called when Zoom In button is clicked
     */
    void onZoomIn();

    /**
     * @brief onZoomOut called when Zoom Out button is clicked
     */
    void onZoomOut();
};

#endif // PLOTWIDGET_H
