#include "../../include/shapes/RegularPolygonShape.h"

RegularPolygonShape::RegularPolygonShape(QObject* parent) : 
    Shape(parent), m_center(0, 0), m_radius(0), m_sides(3) {}

RegularPolygonShape::RegularPolygonShape(const QPoint& center, int radius, int sides, QObject* parent) : 
    Shape(parent), m_center(center), m_radius(radius), m_sides(sides) {}

void RegularPolygonShape::draw(QPainter* painter) {
    if (m_sides < 3 || m_radius <= 0) return;

    painter->save();

    QPen pen(m_penColor, m_penWidth, m_penStyle);
    painter->setPen(pen);
    painter->setBrush(m_fillColor);

    QPolygon polygon = createPolygon();
    painter->drawPolygon(polygon);

    if (m_selected) {
        QRect selectionRect = boundingRect().adjusted(-3, -3, 3, 3);
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(selectionRect);

        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(Qt::white);
        for (const QPoint &p : polygon) {
            painter->drawEllipse(p, 4, 4);
        }
        painter->drawEllipse(m_center, 6, 6);
    }

    painter->restore();
}

void RegularPolygonShape::update(const QPoint& toPoint) {
    m_radius = qMax(1, static_cast<int>(QLineF(m_center, toPoint).length()));
    emit shapeChanged();
}

bool RegularPolygonShape::contains(const QPoint& point) const {
    if (m_sides < 3 || m_radius <= 0) return false;
    
    if (m_selected) {
        QPolygon polygon = createPolygon();
        for (const QPoint &p : polygon) {
            if (QRect(p.x()-5, p.y()-5, 10, 10).contains(point)) {
                return true;
            }
        }
        if (QRect(m_center.x()-7, m_center.y()-7, 14, 14).contains(point)) {
            return true;
        }
    }
    
    return createPolygon().containsPoint(point, Qt::OddEvenFill);
}

void RegularPolygonShape::move(const QPoint& offset){
    m_center += offset;
    emit shapeChanged();
}

void RegularPolygonShape::rotate(double angle){
    m_rotationAngle = angle;
    emit shapeChanged();
}

void RegularPolygonShape::scale(double factor){
    m_radius = qMax(1, static_cast<int>(m_radius * factor));
    emit shapeChanged();
}

QRect RegularPolygonShape::boundingRect() const {
    return QRect(m_center.x() - m_radius - m_penWidth,
                m_center.y() - m_radius - m_penWidth,
                2 * m_radius + 2 * m_penWidth,
                2 * m_radius + 2 * m_penWidth);
}

QJsonObject RegularPolygonShape::toJson() const{
    QJsonObject json = Shape::toJson();
    json["type"] = "regular_polygon";
    json["centerX"] = m_center.x();
    json["centerY"] = m_center.y();
    json["radius"] = m_radius;
    json["sides"] = m_sides;
    return json;
}

void RegularPolygonShape::fromJson(const QJsonObject &json) {
    Shape::fromJson(json);
    if(json.contains("centerX") && json.contains("centerY"))
        m_center = QPoint(json["centerX"].toInt(), json["centerY"].toInt());
    if(json.contains("radius"))
        m_radius = json["radius"].toInt();
    if(json.contains("sides"))
        m_sides = json["sides"].toInt();
    if(json.contains("rotation"))
        m_rotationAngle = json["rotation"].toDouble();
}

QString RegularPolygonShape::name() const{
    return "Regular polygon";
}

QPoint RegularPolygonShape::position() const{
    return m_center - QPoint(m_radius, m_radius);
}

QPoint RegularPolygonShape::center() const{
    return m_center;
}

int RegularPolygonShape::radius() const{
    return m_radius;
}

int RegularPolygonShape::sides() const{
    return m_sides;
}

double RegularPolygonShape::angle() const{
    return m_rotationAngle;
}

void RegularPolygonShape::setCenter(const QPoint& point){
    m_center = point;
    emit shapeChanged();
}

void RegularPolygonShape::setRadius(int radius) {
    m_radius = qMax(1, radius);
    emit shapeChanged();
}

void RegularPolygonShape::setSides(int sides) {
    m_sides = qMax(3, sides);
    emit shapeChanged();
}

void RegularPolygonShape::setRotation(double angle) {
    m_rotationAngle = angle;
    emit shapeChanged();
}

QPolygon RegularPolygonShape::createPolygon() const{
    QPolygon polygon;
    double angleStep = 2 * M_PI / m_sides;
    
    for (int i = 0; i < m_sides; ++i) {
        double angle = m_rotationAngle * M_PI / 180 + i * angleStep;
        int x = m_center.x() + m_radius * cos(angle);
        int y = m_center.y() + m_radius * sin(angle);
        polygon << QPoint(x, y);
    }
    
    return polygon;
}
