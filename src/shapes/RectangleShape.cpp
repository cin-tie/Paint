#include "../../include/shapes/RectangleShape.h"

RectangleShape::RectangleShape(const QRect& rect, QObject* parent) :
    Shape(parent), m_rect(rect) {}

RectangleShape::RectangleShape(const QPoint& topLeft, const QPoint& bottomRight, QObject* parent) :
    Shape(parent), m_rect(QRect(topLeft, bottomRight).normalized()) {}

void RectangleShape::draw(QPainter* painter){
    qDebug() << "Paint rect";   
    painter->save();

    QPen pen(m_penColor, m_penWidth, m_penStyle);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    if (qFuzzyIsNull(m_rotationAngle)) {
        painter->drawRect(m_rect);
    } else {
        painter->translate(rotationCenter());
        painter->rotate(m_rotationAngle);
        painter->translate(-rotationCenter());
        painter->drawRect(m_rect);
        painter->resetTransform();
    }

    if (m_selected) {
        QRect selectionRect = axisAlignedBoundingRect().adjusted(-m_penWidth, -m_penWidth, m_penWidth, m_penWidth);
        
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(selectionRect);

        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(Qt::white);
        
        QPolygonF poly = rotatedPolygon();
        for (const QPointF &point : poly) {
            painter->drawEllipse(point, 4, 4);
        }
        
        painter->drawEllipse(rotationCenter(), 6, 6);
    }

    painter->restore();
}

void RectangleShape::update(const QPoint& toPoint){
    if(m_rect.bottomRight() != toPoint){
        m_rect.setBottomRight(toPoint);
        m_rect = m_rect.normalized();
        emit shapeChanged();
    }
}

bool RectangleShape::contains(const QPoint& point) const{
    if(m_selected){
        QPolygonF poly = rotatedPolygon();
        for(const QPointF &p : poly){
            if(QRectF(p.x() - 5, p.y() - 5, 10, 10).contains(point)){
                return true;
            }
        }
        
        QPointF center = rotationCenter();
        if (QRectF(center.x() - 7, center.y() - 7, 14, 14).contains(point)) {
            return true;
        }
    }
    if (qFuzzyIsNull(m_rotationAngle)) {
        return m_rect.contains(point);
    } else {
        return rotatedPolygon().containsPoint(point, Qt::OddEvenFill);
    }
}

void RectangleShape::move(const QPoint &offset){
    m_rect.translate(offset);
    emit shapeChanged();
}

void RectangleShape::rotate(double angle){
    m_rotationAngle = angle;
    while(m_rotationAngle >= 360.0)
        m_rotationAngle -= 360.0;
    while(m_rotationAngle < 0.0)
        m_rotationAngle += 360.0;
    emit shapeChanged();
}

void RectangleShape::scale(double factor){
    QPoint center = m_rect.center();
    m_rect.setWidth(m_rect.width() * factor);
    m_rect.setHeight(m_rect.height() * factor);
    m_rect.moveCenter(center);
    emit shapeChanged();
}

QRect RectangleShape::boundingRect() const {
    return axisAlignedBoundingRect();
}


QRect RectangleShape::axisAlignedBoundingRect() const{
    if(qFuzzyIsNull(m_rotationAngle)){
        return m_rect;
    }
    QPolygonF poly = rotatedPolygon();
    QRectF rect = poly.boundingRect();
    return rect.toRect();
}

QJsonObject RectangleShape::toJson() const{
    QJsonObject json = Shape::toJson();
    json["type"] = "rectangle";
    json["x"] = m_rect.x();
    json["y"] = m_rect.y();
    json["width"] = m_rect.width();
    json["height"] = m_rect.height();
}

void RectangleShape::fromJson(const QJsonObject& json){
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

QString RectangleShape::name() const{ 
    return "Rectangle"; 
}

QPoint RectangleShape::position() const{
    return m_rect.topLeft(); 
}

QRect RectangleShape::rect() const{
    return m_rect;
}

void RectangleShape::setRect(const QRect& rect){
    if(m_rect != rect){
        m_rect = rect;
        emit shapeChanged();
    }
}

int RectangleShape::width() const{
    return m_rect.width();
}

int RectangleShape::height() const{
    return m_rect.height();
}

void RectangleShape::setWidth(int width){
    if (m_rect.width() != width) {
        m_rect.setWidth(width);
        emit shapeChanged();
    }
}

void RectangleShape::setHeight(int height){
    if(m_rect.height() != height){
        m_rect.setHeight(height);
        emit shapeChanged();
    }
}

QPolygonF RectangleShape::rotatedPolygon() const{
    if (qFuzzyIsNull(m_rotationAngle)) {
        return QPolygonF(QRectF(m_rect));
    }
    QPolygonF polygon;
    polygon << m_rect.topLeft() << m_rect.topRight() << m_rect.bottomLeft() << m_rect.bottomRight();
    QPointF center = rotationCenter();
    QTransform transform;
    transform.translate(center.x(), center.y());
    transform.rotate(m_rotationAngle);
    transform.translate(-center.x(), -center.y());

    return transform.map(polygon);
}

QPointF RectangleShape::rotationCenter() const{
    return m_rect.center();
}