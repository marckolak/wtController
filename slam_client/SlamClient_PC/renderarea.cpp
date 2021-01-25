#include "renderarea.h"
#include <QResizeEvent>
#include "settings.h"
#include <QDebug>
#include <QPainter>
#include <QRectF>
#include <QPalette>
#include <vector>
#include <math.h>

RenderArea::RenderArea(QWidget *parent) : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    lim=8.0;
    //    calcRenderParams();
}


void RenderArea::zoomIn(void)
{
    lim = lim - 1;
    this->calcRenderParams();
        this->repaint();
}

void RenderArea::zoomOut(void)
{
    lim = lim+1;
    this->calcRenderParams();
    this->repaint();
}

void RenderArea::resizeEvent(QResizeEvent* resizedEvent)
{
    QWidget::resizeEvent(resizedEvent);
    resizedEvent->accept();

    // recalcluate renderArea parameters (center etc.)
    if(this->height() > 40)
        this->calcRenderParams();

}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    // create painter
    QPainter p(this);
    QPen* penWall = new QPen(QColor(120,120,120), 1);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(*penWall);

    // plot axes
    this->plotAxes(&p);


    double x[9] = {0, 2, -2, 0, 0, 1, 1, -1, -1};
    double y[9] = {0, 0, 0,  2, -2, 1, -1, -1, 1};
    QPen* penPoint = new QPen(QColor(255,120,120), 10);\
    p.setPen(*penPoint);
    for(int i=0; i<9; i++)
    {
        double rx = this->rX(x[i]);
        double ry = this->rY(y[i]);
        p.drawPoint(rx,ry);
    }
}


void RenderArea::plotAxes(QPainter* p)
{

    // draw lines
    p->drawLine(25, 5, 25, this->height()-25);

    p->drawLine(25, this->height()-25, this->width()-2, this->height()-25);

    // set font
    QFont font = p->font();
    font.setPointSizeF(11);
    p->setFont(font);

    // draw x ticks
    for(auto const& xtick: xTicks)
    {
        p->drawText(QRectF(rX(xtick)-20,
                            this->height()-20, 40, 20),
                    Qt::AlignHCenter,
                    QString::number(xtick));

        p->drawLine(rX(xtick), this->height()-25, rX(xtick), this->height()-28);

    }


    // draw y ticks
    for(auto const& ytick: yTicks)
    {
        p->drawText(QRectF(5,rY(ytick)-20, 15, 40),
                    Qt::AlignVCenter,
                    QString::number(ytick));

        p->drawLine(25, this->rY(ytick), 28, rY(ytick));
    }







}

void RenderArea::calcRenderParams()
{

    rHeight= this-> height() - 30;
    rWidth = this-> width() - 30;

    rAspect = (double)rWidth/rHeight;

    if(rWidth>=rHeight)
    {
        limX = lim;
        limY = lim/rAspect;
    }

    else {
        limX = lim*rAspect;
        limY = lim;
    }

    rCenterX = 30 + rWidth/2;
    rCenterY = rHeight/2;

    xTicks = getTicksX();
    yTicks = getTicksY();
}

double RenderArea::rX(double x)
{
    double xr = (x-cX)* (rWidth/limX)+rCenterX;
//    qDebug() << rAspect;

    return xr;
}


double RenderArea::rY(double y)
{
    double yr = (cY-y)* (rHeight/limY)+ rCenterY;
    return yr;
}

double RenderArea::mX(double xr)
{
    double mx = (xr-rCenterX)*(limX/rWidth) + cX;
    return mx;
}

double RenderArea::mY(double yr)
{
    double my = -(yr-rCenterY)*(limY/rHeight) + cY;
    return my;
}


std::vector<int> RenderArea::getTicksX()
{
    int xMin = std::round(mX(30));
    int xMax = std::round(mX(rWidth+30));

    std::vector<int> xTicks;

    for(int i=0; i<= xMax-xMin-2; i++)
        xTicks.push_back(xMin+1 + i);

    return xTicks;
}

std::vector<int> RenderArea::getTicksY()
{
    int yMax = std::round(mY(0));
    int yMin = std::round(mY(rHeight));

    std::vector<int> yTicks;

    for(int i=0; i<= yMax-yMin-2; i++)
        yTicks.push_back(yMin+1 + i);

    return yTicks;
}
