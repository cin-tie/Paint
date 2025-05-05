#ifndef REGULARPOLYGONSHAPE_H
#define REGULARPOLYGONSHAPE_H

#include "Shape.h"

class RegularPolygonShape : public Shape{

    Q_OBJECT

    public:
        explicit RegularPolygonShape(QObject* parent = nullptr);
        explicit RegularPolygonShape(const QPoint& center, int radius, int sides, QObject* parent = nullptr);

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

        QPoint center() const;
        int radius() const;
        int sides() const;
        double angle() const;
        void setCenter(const QPoint& center);
        void setRadius(int radius);
        void setSides(int sides);
        void setRotation(double angle);
    private:
        QPoint m_center;
        int m_radius;
        int m_sides;
        
        QPolygon createPolygon() const;
};

#endif