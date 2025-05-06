#include "../../include/shapes/PolygonShape.h"

PolygonShape::PolygonShape(QObject* parent) : Shape(parent) {}

PolygonShape::PolygonShape(const QPolygon& polygon, QObject* parent) : 
    Shape(parent), m_polygon(polygon), m_closed(true) {
    updateBoundingRect();
}

void PolygonShape::draw(QPainter* painter){
    if (m_polygon.size() < 2)
        return;

    painter->save();

    QPen pen(m_penColor, m_penWidth, m_penStyle);
    pen.setJoinStyle(Qt::MiterJoin);
    painter->setPen(pen);
    painter->setBrush(m_fillColor);

    if (m_closed) {
        painter->drawPolygon(m_polygon);
    } else {
        painter->drawPolyline(m_polygon);
    }

    if (m_selected) {
        QRect selectionRect = axisAlignedBoundingRect().adjusted(-3, -3, 3, 3);
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(selectionRect);

        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(Qt::white);
        for (const QPoint &p : m_polygon) {
            painter->drawEllipse(p, 4, 4);
        }
    }

    painter->restore();
}

void PolygonShape::update(const QPoint& toPoint){
    m_polygon << toPoint;
    updateBoundingRect();
    emit shapeChanged();
}

bool PolygonShape::contains(const QPoint& point) const {
    if (m_selected) {
        for (const QPoint &p : m_polygon) {
            if (QRect(p.x()-5, p.y()-5, 10, 10).contains(point)) {
                return true;
            }
        }
    }
    
    if (m_closed) {
        return m_polygon.containsPoint(point, Qt::OddEvenFill);
    } else {
        return isPointNearEdge(point);
    }
}

void PolygonShape::move(const QPoint& offset) {
    m_polygon.translate(offset);
    updateBoundingRect();
    emit shapeChanged();
}

void PolygonShape::rotate(double angle) {
    if (m_polygon.isEmpty()) return;
    
    QPoint center = m_boundingRect.center();
    QTransform transform;
    transform.translate(center.x(), center.y());
    transform.rotate(angle);
    transform.translate(-center.x(), -center.y());
    
    m_polygon = transform.map(m_polygon);
    updateBoundingRect();
    emit shapeChanged();
}

void PolygonShape::scale(double factor) {
    if (m_polygon.isEmpty()) return;
    
    QPoint center = m_boundingRect.center();
    QTransform transform;
    transform.translate(center.x(), center.y());
    transform.scale(factor, factor);
    transform.translate(-center.x(), -center.y());
    
    m_polygon = transform.map(m_polygon);
    updateBoundingRect();
    emit shapeChanged();
}

QRect PolygonShape::boundingRect() const {
    return axisAlignedBoundingRect();
}

QJsonObject PolygonShape::toJson() const {
    QJsonObject json = Shape::toJson();
    json["type"] = "polygon";
    json["closed"] = m_closed;
    
    QJsonArray pointsArray;
    for (const QPoint &p : m_polygon) {
        QJsonObject pointObj;
        pointObj["x"] = p.x();
        pointObj["y"] = p.y();
        pointsArray.append(pointObj);
    }
    
    json["points"] = pointsArray;
    return json;
}

void PolygonShape::fromJson(const QJsonObject &json) {
    Shape::fromJson(json);
    
    m_polygon.clear();
    if(json.contains("points")){
        QJsonArray pointsArray = json["points"].toArray();
        for (const QJsonValue &val : pointsArray) {
            QJsonObject pointObj = val.toObject();
            if(pointObj.contains("x") && pointObj.contains("y"))
                m_polygon << QPoint(pointObj["x"].toInt(), pointObj["y"].toInt());
        }
        if(json.contains("closed"))
            m_closed = json["closed"].toBool();
    }
    updateBoundingRect();
}

QString PolygonShape::name() const{
    return "Polygon";
}

QPoint PolygonShape::position() const {
    return m_boundingRect.topLeft();
}

void PolygonShape::addPoint(const QPoint &point) {
    m_polygon << point;
    updateBoundingRect();
    emit shapeChanged();
}

void PolygonShape::closePolygon() {
    if (!m_closed && m_polygon.size() > 2) {
        m_closed = true;
        emit shapeChanged();
    }
}

const QPolygon& PolygonShape::polygon() const {
    return m_polygon;
}

void PolygonShape::setPolygon(const QPolygon& polygon) {
    m_polygon = polygon;
    updateBoundingRect();
    emit shapeChanged();
}

bool PolygonShape::isClosed() const {
    return m_closed;
}

int PolygonShape::pointCount() const {
    return m_polygon.size();
}

void PolygonShape::updateBoundingRect() {
    m_boundingRect = m_polygon.boundingRect();
}

QRect PolygonShape::axisAlignedBoundingRect() const {
    return m_polygon.boundingRect().adjusted(-m_penWidth, -m_penWidth, m_penWidth, m_penWidth);
}

bool PolygonShape::isPointNearEdge(const QPoint& point) const {
    if (m_polygon.size() < 2) return false;
    
    for (int i = 1; i < m_polygon.size(); ++i) {
        QLineF line(m_polygon[i-1], m_polygon[i]);
        qreal length = line.length();
        if (qFuzzyIsNull(length)) continue;
        
        QPointF vec = line.p2() - line.p1();
        QPointF pointVec = point - line.p1();
        
        qreal t = QPointF::dotProduct(pointVec, vec) / (length * length);
        t = qBound(0.0, t, 1.0);
        
        QPointF projected = line.p1() + t * vec;
        qreal distance = QLineF(projected, point).length();
        
        if (distance <= m_penWidth / 2 + 2) {
            return true;
        }
    }
    return false;
}