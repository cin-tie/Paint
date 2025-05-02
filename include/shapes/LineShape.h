#ifndef LINESHAPE_H
#define LINESHAPE_H

#include "Shape.h"

class LineShape : public Shape{

    Q_OBJECT

    public:
        explicit LineShape(const QPoint& startPoint = QPoint(), const QPoint& endPoint = QPoint(), QObject* parent = nullptr);

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

        QPoint startPoint() const;
        QPoint endPoint() const;
        void setStartPoint(const QPoint& point);
        void setEndPoint(const QPoint& point);
        double length() const;
        double angle() const;

    private:
        QPoint m_startPoint;
        QPoint m_endPoint;

        double distanceToLine(const QPoint& point) const;
        QPointF rotatePoint(const QPointF& point, const QPointF& center, double angle) const;
};

#endif