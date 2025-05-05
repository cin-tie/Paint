#include "../../include/shapes/EllipseShape.h"

EllipseShape::EllipseShape(const QRect& rect, QObject* parent) :
    Shape(parent), m_rect(rect) {}

EllipseShape::EllipseShape(const QPoint& center, int rx, int ry, QObject* parent) :
    Shape(parent), m_rect(center.x() - rx, center.y() - ry, rx * 2, ry * 2) {}

void EllipseShape::draw(QPainter* painter){
    painter->save();

    QPen pen(m_penColor, m_penWidth, m_penStyle);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    if (qFuzzyIsNull(m_rotationAngle)) {
        painter->drawEllipse(m_rect);
    } else {
        painter->translate(rotationCenter());
        painter->rotate(m_rotationAngle);
        painter->translate(-rotationCenter());
        painter->drawEllipse(m_rect);
        painter->resetTransform();
    }

    if (m_selected) {
        QRect selectionRect = axisAlignedBoundingRect().adjusted(-m_penWidth, -m_penWidth, m_penWidth, m_penWidth);
        
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(selectionRect);

        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(Qt::white);
        
        QRectF rect = m_rect;
        if (!qFuzzyIsNull(m_rotationAngle)) {
            QPolygonF poly = rotatedPolygon();
            QPointF top = (poly[0] + poly[1]) / 2;
            QPointF right = (poly[1] + poly[2]) / 2;
            QPointF bottom = (poly[2] + poly[3]) / 2;
            QPointF left = (poly[3] + poly[0]) / 2;
            
            painter->drawEllipse(top, 4, 4);
            painter->drawEllipse(right, 4, 4);
            painter->drawEllipse(bottom, 4, 4);
            painter->drawEllipse(left, 4, 4);
        } else {
            painter->drawEllipse(rect.left(), rect.center().y(), 4, 4);
            painter->drawEllipse(rect.right(), rect.center().y(), 4, 4);
            painter->drawEllipse(rect.center().x(), rect.top(), 4, 4);
            painter->drawEllipse(rect.center().x(), rect.bottom(), 4, 4);
        }
        
        painter->drawEllipse(rotationCenter(), 6, 6);
    }

    painter->restore();
}

void EllipseShape::update(const QPoint& toPoint){
    if(m_rect.bottomRight() != toPoint){
        m_rect.setBottomRight(toPoint);
        m_rect = m_rect.normalized();
        emit shapeChanged();
    }
}

bool EllipseShape::contains(const QPoint& point) const{
    if (m_selected) {
        QRectF rect = m_rect;
        if (!qFuzzyIsNull(m_rotationAngle)) {
            QPolygonF poly = rotatedPolygon();
            QPointF top = (poly[0] + poly[1]) / 2;
            QPointF right = (poly[1] + poly[2]) / 2;
            QPointF bottom = (poly[2] + poly[3]) / 2;
            QPointF left = (poly[3] + poly[0]) / 2;
            
            if (QRectF(top.x() - 5, top.y() - 5, 10, 10).contains(point) ||
                QRectF(right.x() - 5, right.y() - 5, 10, 10).contains(point) ||
                QRectF(bottom.x() - 5, bottom.y() - 5, 10, 10).contains(point) ||
                QRectF(left.x() - 5, left.y() - 5, 10, 10).contains(point)) {
                return true;
            }
        } else {
            if (QRectF(rect.left() - 5, rect.center().y() - 5, 10, 10).contains(point) ||
                QRectF(rect.right() - 5, rect.center().y() - 5, 10, 10).contains(point) ||
                QRectF(rect.center().x() - 5, rect.top() - 5, 10, 10).contains(point) ||
                QRectF(rect.center().x() - 5, rect.bottom() - 5, 10, 10).contains(point)) {
                return true;
            }
        }
        
        QPointF center = rotationCenter();
        if (QRectF(center.x() - 7, center.y() - 7, 14, 14).contains(point)) {
            return true;
        }
    }

    if (qFuzzyIsNull(m_rotationAngle)) {
        return isPointOnEllipse(point);
    } else {
        return rotatedPolygon().containsPoint(point, Qt::OddEvenFill);
    }
}

void EllipseShape::move(const QPoint& offset){
    m_rect.translate(offset);
    emit shapeChanged();
}

void EllipseShape::rotate(double angle){
    m_rotationAngle = angle;
    while(m_rotationAngle >= 360.0)
        m_rotationAngle -= 360.0;
    while(m_rotationAngle < 0.0)
        m_rotationAngle += 360.0;
    emit shapeChanged();
}

void EllipseShape::scale(double factor){
    QPoint center = m_rect.center();
    m_rect.setWidth(m_rect.width() * factor);
    m_rect.setHeight(m_rect.height() * factor);
    m_rect.moveCenter(center);
    emit shapeChanged();
}

QRect EllipseShape::boundingRect() const {
    return axisAlignedBoundingRect();
}


QRect EllipseShape::axisAlignedBoundingRect() const{
    if(qFuzzyIsNull(m_rotationAngle)){
        return m_rect;
    }
    QPolygonF poly = rotatedPolygon();
    QRectF rect = poly.boundingRect();
    return rect.toRect();
}

QJsonObject EllipseShape::toJson() const{
    QJsonObject json = Shape::toJson();
    json["type"] = "ellipse";
    json["x"] = m_rect.x();
    json["y"] = m_rect.y();
    json["width"] = m_rect.width();
    json["height"] = m_rect.height();
}

void EllipseShape::fromJson(const QJsonObject& json){
    Shape::fromJson(json);
    if(json.contains("x"))
        m_rect.setX(json["x"].toInt());
    if(json.contains("y"))
        m_rect.setY(json["y"].toInt());
    if(json.contains("width"))
        m_rect.setWidth(json["width"].toInt());
    if(json.contains("height"))
        m_rect.setWidth(json["height"].toInt());
}

QString EllipseShape::name() const{ 
    return "Ellipse"; 
}

QPoint EllipseShape::position() const{
    return m_rect.topLeft(); 
}

QRect EllipseShape::rect() const{
    return m_rect;
}

void EllipseShape::setRect(const QRect& rect){
    if(m_rect != rect){
        m_rect = rect;
        emit shapeChanged();
    }
}

QPoint EllipseShape::center() const{
    return m_rect.center();
}

int EllipseShape::radiusX() const{
    return m_rect.width() / 2;
}

int EllipseShape::radiusY() const{
    return m_rect.height() / 2;
}

void EllipseShape::setRadiusX(int rx)
{
    if (radiusX() != rx) {
        m_rect.setWidth(rx * 2);
        emit shapeChanged();
    }
}

void EllipseShape::setRadiusY(int ry)
{
    if (radiusY() != ry) {
        m_rect.setHeight(ry * 2);
        emit shapeChanged();
    }
}

QPolygonF EllipseShape::rotatedPolygon() const{
    const int points = 1024;
    QPolygonF polygon;
    
    double rx = m_rect.width() / 2.0;
    double ry = m_rect.height() / 2.0;
    QPointF center = m_rect.center();
    
    for (int i = 0; i < points; ++i) {
        double angle = 2 * M_PI * i / points;
        QPointF point(
            center.x() + rx * cos(angle),
            center.y() + ry * sin(angle)
        );
        polygon << point;
    }
    
    if (!qFuzzyIsNull(m_rotationAngle)) {
        QTransform transform;
        transform.translate(center.x(), center.y());
        transform.rotate(m_rotationAngle);
        transform.translate(-center.x(), -center.y());
        polygon = transform.map(polygon);
    }
    
    return polygon;
}

QPointF EllipseShape::rotationCenter() const{
    return m_rect.center();
}

bool EllipseShape::isPointOnEllipse(const QPoint& point) const{
    QPointF center = m_rect.center();
    double rx = m_rect.width() / 2.0;
    double ry = m_rect.height() / 2.0;

    if(rx <= 0 || ry <= 0)
        return false;
    
    double dx = point.x() - center.x();
    double dy = point.y() - center.y();
    
    double distance = qPow(dx / rx, 2) + qPow(dy / ry, 2);
    double minDist = qPow(1.0 - m_penWidth / (2 * qMin(rx, ry)), 2);
    double maxDist = qPow(1.0 + m_penWidth / (2 * qMin(rx, ry)), 2);

    return distance >= minDist && distance <= maxDist;
}