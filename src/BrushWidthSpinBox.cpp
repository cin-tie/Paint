#include "../include/BrushWidthSpinBox.h"
#include <QPainter>

BrushWidthSpinBox::BrushWidthSpinBox(QWidget *parent) : QSpinBox(parent)
{
    setRange(1, 20);
    setValue(1);
    setSuffix(" px");
}

void BrushWidthSpinBox::paintEvent(QPaintEvent *event)
{
    QSpinBox::paintEvent(event);
    
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, value(), Qt::SolidLine, Qt::RoundCap));
    int y = height() / 2;
    painter.drawLine(5, y, width() - 5, y);
}