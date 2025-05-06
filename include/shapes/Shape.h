#ifndef SHAPE_H
#define SHAPE_H

#include <QObject>
#include <QPainter>
#include <QPoint>
#include <QColor>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

class Shape : public QObject
{
    Q_OBJECT

    public:
        explicit Shape(QObject* parent = nullptr);
        virtual ~Shape() = default;

        virtual void draw(QPainter* painter) = 0;
        virtual void update(const QPoint& toPoint) = 0;
        virtual bool contains(const QPoint& point) const = 0;
        virtual void move(const QPoint& offset);
        virtual void rotate(double angle);
        virtual void scale(double factor);
        virtual QRect boundingRect() const = 0;

        void setPenColor(const QColor& color);
        void setPenWidth(int width);
        void setFillColor(const QColor& color);
        void setPenStyle(Qt::PenStyle style);
        void setRotationAngle(double angle);

        QColor penColor() const;
        int penWidth() const;
        QColor fillColor() const;
        Qt::PenStyle penStyle() const;
        double rotationAngle() const;

        virtual QJsonObject toJson() const;
        virtual void fromJson(const QJsonObject& json);

        virtual QString name() const = 0;
        virtual QPoint position() const = 0;

        bool isSelected() const;
        void setSelected(bool selected);
        bool isAnimating() const;
        void setAnimating(bool animating);

    signals:
        void shapeChanged();

    protected:
        QColor m_penColor;
        int m_penWidth;
        QColor m_fillColor;
        Qt::PenStyle m_penStyle;
        bool m_selected;
        bool m_animating;
        double m_rotationAngle = 0.0;
};

#endif