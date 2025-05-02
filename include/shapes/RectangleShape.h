#ifndef RECTANGLESHAPE_H
#define RECTANGLESHAPE_H

#include "Shape.h"

class RectangleShape : public Shape{

    Q_OBJECT

    public:
        explicit RectangleShape(const QRect& rect = QRect(), QObject* parent = nullptr);
        explicit RectangleShape(const QPoint& topLeft, const QPoint& bottomRight, QObject* parent = nullptr);

        void draw(QPainter* painter) override;
        void update(const QPoint& toPoint) override;
        bool contains(const QPoint& point) const override;
        void move(const QPoint& offset) override;
        void rotate(double angle) override;
        void scale(double factor) override;
        QRect boundingRect() const override;

        QJsonObject toJson() const override;
        void fromJson(const QJsonObject& json) override;

        QString name() const override;
        QPoint position() const override;

        QRect rect() const;
        void setRect(const QRect& rect);
        int width() const;
        int height() const;
        void setWidth(int width);
        void setHeight(int height); 

    private:
        QRect m_rect;

        QPolygonF rotatedPolygon() const;
        QPointF rotationCenter() const;
        QRect axisAlignedBoundingRect() const;
};

#endif