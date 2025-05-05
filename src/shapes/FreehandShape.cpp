#include "../../include/shapes/FreehandShape.h"

FreehandShape::FreehandShape(QObject* parent) : Shape(parent) {}

FreehandShape::FreehandShape(const QVector<QPoint>& points, QObject* parent) :
    Shape(parent), m_points(points) {
    updateBoundingRect();
}

void FreehandShape::draw(QPainter *painter)
{
    if (m_points.size() < 2)
        return;

    painter->save();

    QPen pen(m_penColor, m_penWidth, m_penStyle);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    painter->drawPolyline(m_points.data(), m_points.size());

    if (m_selected) {
        QRect selectionRect = boundingRect().adjusted(-m_penWidth, -m_penWidth, m_penWidth, m_penWidth);
        
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(selectionRect);

        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(Qt::white);
        
        if (!m_points.isEmpty()) {
            painter->drawEllipse(m_points.first(), 4, 4);
            painter->drawEllipse(m_points.last(), 4, 4);
        }
        
        if (m_points.size() > 10) {
            int step = m_points.size() / 5;
            for (int i = step; i < m_points.size(); i += step) {
                painter->drawEllipse(m_points[i], 4, 4);
            }
        } else {
            for (const QPoint &p : m_points) {
                painter->drawEllipse(p, 4, 4);
            }
        }
    }

    painter->restore();
}


QRect FreehandShape::axisAlignedBoundingRect() const{
    if(m_points.isEmpty()){
        return QRect();
    }
    int minX = m_points[0].x();
    int minY = m_points[0].y();
    int maxX = minX;
    int maxY = minY;

    for(const QPoint& p : m_points){
        minX = qMin(minX, p.x());
        minY = qMin(minY, p.y());
        maxX = qMax(maxX, p.x());
        maxY = qMax(maxY, p.y());
    }

    return QRect(QPoint(minX,minY), QPoint(maxX, maxY));
}

void FreehandShape::update(const QPoint& toPoint){
    m_points.push_back(toPoint);
    updateBoundingRect();
    emit shapeChanged();
}

bool FreehandShape::contains(const QPoint& point) const{
    if(m_selected){
        for(const QPoint& p : m_points){
            if(QRect(p.x() - 5, p.y() - 5, 10, 10).contains(point))
                return true;
        }

        if(axisAlignedBoundingRect().adjusted(-m_penWidth, -m_penWidth, m_penWidth, m_penWidth).contains(point));
    }
    return isPointNearPolyline(point);
}

void FreehandShape::move(const QPoint& offset){
    for(QPoint& p : m_points){
        p += offset;
    }
    updateBoundingRect();
    emit shapeChanged();
}

void FreehandShape::rotate(double angle){
    QPoint center = m_boundingRect.center();
    QTransform transform;
    transform.translate(center.x(), center.y());
    transform.rotate(angle); 
    transform.translate(-center.x(), -center.y());

    applyTransform(transform);
    emit shapeChanged();
}


void FreehandShape::scale(double factor)
{
    QPoint center = m_boundingRect.center();
    QTransform transform;
    transform.translate(center.x(), center.y());
    transform.scale(factor, factor);
    transform.translate(-center.x(), -center.y());
    
    applyTransform(transform);
    emit shapeChanged();
}

QRect FreehandShape::boundingRect() const{
    return axisAlignedBoundingRect();
}

QJsonObject FreehandShape::toJson() const{
    QJsonObject json = Shape::toJson();
    
    json["type"] = "freehand";
    QJsonArray pointsArray;
    for(const QPoint& p : m_points){
        QJsonObject pointObject;
        pointObject["x"] = p.x();
        pointObject["y"] = p.y();
        pointsArray.push_back(pointObject);
    }

    json["points"] = pointsArray;
    return json;
}

void FreehandShape::fromJson(const QJsonObject& json){
    Shape::fromJson(json);
    if(json.contains("points")){
        m_points.clear();
        QJsonArray pointArray = json["points"].toArray();
        for(const QJsonValue& val : pointArray){
            QJsonObject pointObject = val.toObject();
            if(pointObject.contains("x") && pointObject.contains("y")){
                m_points.push_back(QPoint(pointObject["x"].toInt(), pointObject["y"].toInt()));
            }
        }
        updateBoundingRect();
    }
}

QString FreehandShape::name() const{
    return "Freehand";
}

QPoint FreehandShape::position() const{
    return axisAlignedBoundingRect().topLeft();
}

void FreehandShape::addPoint(const QPoint& point){
    m_points.push_back(point);
    updateBoundingRect();
    emit shapeChanged();
}

void FreehandShape::clearPoints(){
    m_points.clear();
    updateBoundingRect();
    emit shapeChanged();
}

const QVector<QPoint>& FreehandShape::points() const{
    return m_points;
}

void FreehandShape::setPoints(const QVector<QPoint>& points){
    m_points = points;
    updateBoundingRect();
    emit shapeChanged();
}

void FreehandShape::simplify(double tolerance){
    if(m_points.size() < 3)
        return;

    QVector<QPoint> simplified;
    simplified.push_back(m_points[0]);

    for(size_t i = 1; i < m_points.size() - 1; ++i){
        QPoint prev = m_points[i - 1];
        QPoint current = m_points[i];
        QPoint next = m_points[i + 1];

        double a = QLineF(prev, next).length();
        double b = QLineF(prev, current).length();
        double c = QLineF(current, next).length();

        double p = (a + b + c) / 2;
        double S = qSqrt(p * (p - a) * (p - b) * (p - c));

        double distance = (2 * S) / a;

        if(distance > tolerance){
            simplified.push_back(current);
        }
    }

    simplified.push_back(m_points[m_points.size() - 1]);
    m_points = simplified;
    updateBoundingRect();
    emit shapeChanged();
}

void FreehandShape::updateBoundingRect() {
    m_boundingRect = axisAlignedBoundingRect();
}

bool FreehandShape::isPointNearPolyline(const QPoint &point) const
{
    if (m_points.size() < 2)
        return false;
    
    for (int i = 1; i < m_points.size(); ++i) {
        const QPoint &p1 = m_points[i-1];
        const QPoint &p2 = m_points[i];
        
        QLineF line(p1, p2);
        double lineLength = line.length();
        if (qFuzzyIsNull(lineLength))
            continue;
        
        QPointF lineVector = p2 - p1;
        QPointF pointVector = point - p1;
        
        double t = (pointVector.x() * lineVector.x() + pointVector.y() * lineVector.y()) / 
                  (lineLength * lineLength);
        
        t = qBound(0.0, t, 1.0);
        
        QPointF projected = p1 + t * lineVector;
        
        double distance = QLineF(projected, point).length();
        
        bool isOnSegment = (projected.x() >= qMin(p1.x(), p2.x()) - m_penWidth &&
                          projected.x() <= qMax(p1.x(), p2.x()) + m_penWidth &&
                          projected.y() >= qMin(p1.y(), p2.y()) - m_penWidth &&
                          projected.y() <= qMax(p1.y(), p2.y()) + m_penWidth);
        
        if (isOnSegment && distance <= m_penWidth / 2 + 2) {
            return true;
        }
    }
    
    return false;
}

void FreehandShape::applyTransform(const QTransform& transform){
    for (QPoint &p : m_points) {
        p = transform.map(p);
    }
    updateBoundingRect();
}