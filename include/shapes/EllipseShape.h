#ifndef ELLIPSESHAPE_H
#define ELLIPSESHAPE_H

#include "Shape.h"

class EllipseShape : public Shape{

    Q_OBJECT

    public:
        explicit EllipseShape(const QRect& rect = QRect(), QObject* parent = nullptr);
        explicit EllipseShape(const QPoint& center, int rx, int ry, QObject* parent = nullptr);

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
        QPoint center() const;
        int radiusX() const;
        int radiusY() const;
        void setRadiusX(int rx);
        void setRadiusY(int ry);
 
    private:
        QRect m_rect;
        
        QPolygonF rotatedPolygon() const;
        QPointF rotationCenter() const;
        QRect axisAlignedBoundingRect() const;
        bool isPointOnEllipse(const QPoint& point) const;
};

#endif