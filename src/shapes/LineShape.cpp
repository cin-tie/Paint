#include "../../include/shapes/LineShape.h"
#include <QPainter>
#include <QPen>
#include <QtMath>
#include <QJsonObject>

LineShape::LineShape(const QPoint& startPoint, const QPoint& endPoint, QObject* parent)
    : Shape(parent), m_startPoint(startPoint), m_endPoint(endPoint) {
    m_rotationAngle = qAtan2(m_endPoint.y() - m_startPoint.y(), m_endPoint.x() - m_startPoint.x());
}

void LineShape::draw(QPainter* painter){
    painter->save();

    QPen pen(m_penColor, m_penWidth, m_penStyle);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    painter->drawLine(m_startPoint, m_endPoint);

    if (m_selected) {
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect().adjusted(-m_penWidth, -m_penWidth, m_penWidth, m_penWidth));
        
        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(Qt::white);
        painter->drawEllipse(m_startPoint, 4, 4);
        painter->drawEllipse(m_endPoint, 4, 4);
    }

    painter->restore();
}

void LineShape::update(const QPoint& toPoint){
    if(m_endPoint != toPoint){
        m_endPoint = toPoint;
        m_rotationAngle = qAtan2(m_endPoint.y() - m_startPoint.y(), m_endPoint.x() - m_startPoint.x());
        emit shapeChanged();
    }
}

bool LineShape::contains(const QPoint& point) const{
    if (m_selected) {
        QRect startMarker(m_startPoint - QPoint(5, 5), QSize(10, 10));
        QRect endMarker(m_endPoint - QPoint(5, 5), QSize(10, 10));
        
        if (startMarker.contains(point) || endMarker.contains(point)) {
            return true;
        }
    }
    
    return distanceToLine(point) <= (m_penWidth / 2 + 3);
}

void LineShape::move(const QPoint& offset){
    m_startPoint += offset;
    m_endPoint += offset;
    emit shapeChanged();
}

void LineShape::rotate(double angle){
    QPoint center = boundingRect().center();
    QPointF newStart = rotatePoint(m_startPoint, center, angle);
    QPointF newEnd = rotatePoint(m_endPoint, center, angle);
    
    m_startPoint = newStart.toPoint();
    m_endPoint = newEnd.toPoint();
    m_rotationAngle = qAtan2(m_endPoint.y() - m_startPoint.y(), m_endPoint.x() - m_startPoint.x());
    emit shapeChanged();
}

void LineShape::scale(double factor){
    QPoint center = boundingRect().center();
    
    m_startPoint = center + (m_startPoint - center) * factor;
    m_endPoint = center + (m_endPoint - center) * factor;
    emit shapeChanged();
}

QRect LineShape::boundingRect() const{
    return QRect(m_startPoint, m_endPoint).normalized();
}

QJsonObject LineShape::toJson() const{
    QJsonObject json = Shape::toJson();
    json["type"] = "line";
    json["startX"] = m_startPoint.x();
    json["startY"] = m_startPoint.y();
    json["endX"] = m_endPoint.x();
    json["endY"] = m_endPoint.y();
    return json;
}

void LineShape::fromJson(const QJsonObject& json){
    Shape::fromJson(json);
    if(json.contains("startX"))
        m_startPoint.setX(json["startX"].toInt());
    if(json.contains("startY"))
        m_startPoint.setY(json["startY"].toInt());
    if(json.contains("endX"))
        m_endPoint.setX(json["endX"].toInt());
    if(json.contains("endY"))
        m_endPoint.setY(json["endY"].toInt());
}

QString LineShape::name() const{
    return "Line";
}

QPoint LineShape::position() const{
    return m_startPoint;
}

QPoint LineShape::startPoint() const{
    return m_startPoint;
}

QPoint LineShape::endPoint() const{
    return m_endPoint;
}

void LineShape::setStartPoint(const QPoint& point){
    if (m_startPoint != point) {
        m_startPoint = point;
        m_rotationAngle = qAtan2(m_endPoint.y() - m_startPoint.y(), m_endPoint.x() - m_startPoint.x());
        emit shapeChanged();
    }
}

void LineShape::setEndPoint(const QPoint& point){
    if (m_endPoint != point) {
        m_endPoint = point;        
        m_rotationAngle = qAtan2(m_endPoint.y() - m_startPoint.y(), m_endPoint.x() - m_startPoint.x());
        emit shapeChanged();
    }
}

double LineShape::length() const{
    return qSqrt(qPow(m_endPoint.x() - m_startPoint.x(), 2) + 
                qPow(m_endPoint.y() - m_startPoint.y(), 2));
}

double LineShape::angle() const{
    return m_rotationAngle;
}

double LineShape::distanceToLine(const QPoint &point) const{
    if (m_startPoint == m_endPoint) {
        return qSqrt(qPow(point.x() - m_startPoint.x(), 2) + 
               qPow(point.y() - m_startPoint.y(), 2));
    }
    
    double lineLength = length();
    
    double projection = ((point.x() - m_startPoint.x()) * (m_endPoint.x() - m_startPoint.x()) + 
                       (point.y() - m_startPoint.y()) * (m_endPoint.y() - m_startPoint.y())) / 
                      (lineLength * lineLength);
    
    if (projection < 0) {
        return qSqrt(qPow(point.x() - m_startPoint.x(), 2) + 
                    qPow(point.y() - m_startPoint.y(), 2));
    } 
    else{
        if (projection > 1) {
            return qSqrt(qPow(point.x() - m_endPoint.x(), 2) + 
                        qPow(point.y() - m_endPoint.y(), 2));
        }
    }
    
    QPointF projPoint(
        m_startPoint.x() + projection * (m_endPoint.x() - m_startPoint.x()),
        m_startPoint.y() + projection * (m_endPoint.y() - m_startPoint.y())
    );
    
    return qSqrt(qPow(point.x() - projPoint.x(), 2) + 
                qPow(point.y() - projPoint.y(), 2));
}

QPointF LineShape::rotatePoint(const QPointF& point, const QPointF& center, double angle) const{
    double s = qSin(angle);
    double c = qCos(angle);
    
    QPointF translated = point - center;
    
    QPointF rotated(
        translated.x() * c - translated.y() * s,
        translated.x() * s + translated.y() * c
    );
    
    return rotated + center;
}