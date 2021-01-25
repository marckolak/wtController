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


private:
    Ui::PlotWidget *ui;

    /** RenderArea Widget, where the map is drawn*/
    RenderArea *renderArea;

private slots:
    void onZoomIn();
    void onZoomOut();
};

#endif // PLOTWIDGET_H
