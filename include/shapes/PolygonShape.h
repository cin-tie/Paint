#ifndef POLYGONSHAPE_H
#define POLYGONSHAPE_H

#include "Shape.h"
#include <QPolygon>

class PolygonShape : public Shape
{
    Q_OBJECT

    public:
        explicit PolygonShape(QObject* parent = nullptr);
        explicit PolygonShape(const QPolygon& polygon, QObject* parent = nullptr);

        void draw(QPainter* painter) override;
        void update(const QPoint& toPoint) override;
        bool contains(const QPoint& point) const override;
        void move(const QPoint& offset) override;
        void rotate(double angle) override;
        void scale(double factor) override;
        QRect boundingRect() const override;

        QJsonObject toJson() const override;
        void fromJson(const QJsonObject &json) override;

        QString name() const override;
        QPoint position() const override;

        void addPoint(const QPoint &point);
        void closePolygon();
        const QPolygon& polygon() const;
        void setPolygon(const QPolygon& polygon);
        bool isClosed() const;
        int pointCount() const;

    private:
        QPolygon m_polygon;
        bool m_closed = false;
        QRect m_boundingRect;

        void updateBoundingRect();  
        QRect axisAlignedBoundingRect() const;
        bool isPointNearEdge(const QPoint& point) const;
};

#endif