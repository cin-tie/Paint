#ifndef FREEHANDSHAPE_H
#define FREEHANDSHAPE_H

#include "Shape.h"
#include <QVector>

class FreehandShape : public Shape{

    Q_OBJECT

    public:
        explicit FreehandShape(QObject* parent = nullptr);
        explicit FreehandShape(const QVector<QPoint>& points, QObject* parent = nullptr);

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

        void addPoint(const QPoint& point);
        void clearPoints();
        const QVector<QPoint>& points() const;
        void setPoints(const QVector<QPoint>& points);
        void simplify(double tolerance = 1.0);

    private:
        QVector<QPoint> m_points;
        QRect m_boundingRect;

        void updateBoundingRect();
        bool isPointNearPolyline(const QPoint& point) const;
        void applyTransform(const QTransform& transform);
        QRect axisAlignedBoundingRect() const;
};

#endif