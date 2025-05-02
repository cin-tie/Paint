#include "../../include/shapes/Shape.h"

Shape::Shape(QObject *parent) 
    : QObject(parent),
      m_penColor(Qt::black),
      m_penWidth(1),
      m_fillColor(Qt::transparent),
      m_penStyle(Qt::SolidLine),
      m_selected(false),
      m_animating(false),
      m_rotationAngle(0.0) {}

void Shape::move(const QPoint& offset){
    Q_UNUSED(offset);
    emit shapeChanged();
}

void Shape::rotate(double angle){
    Q_UNUSED(angle);
    emit shapeChanged();
}

void Shape::scale(double factor){
    Q_UNUSED(factor);
    emit shapeChanged();
}

void Shape::setPenColor(const QColor& color){
    if(m_penColor != color){
        m_penColor = color;
        emit shapeChanged();
    }
}

void Shape::setPenWidth(int width){
    if(m_penWidth != width){
        m_penWidth = width;
        emit shapeChanged();
    }
}

void Shape::setFillColor(const QColor& color){
    if(m_fillColor != color){
        m_fillColor = color;
        emit shapeChanged();
    }
}

void Shape::setPenStyle(Qt::PenStyle style){
    if(m_penStyle != style){
        m_penStyle = style;
        emit shapeChanged();
    }
}

void Shape::setRotationAngle(double angle){
    if(m_rotationAngle != angle){
        m_rotationAngle = angle;
        emit shapeChanged();
    }
}

QColor Shape::penColor() const{
    return m_penColor;
}

int Shape::penWidth() const{
    return m_penWidth;
}

QColor Shape::fillColor() const{
    return m_fillColor;
}

Qt::PenStyle Shape::penStyle() const{
    return m_penStyle;
}

QJsonObject Shape::toJson() const
{
    QJsonObject json;
    json["penColor"] = m_penColor.name();
    json["penWidth"] = m_penWidth;
    json["fillColor"] = m_fillColor.name();
    json["penStyle"] = static_cast<int>(m_penStyle);
    json["rotationAngle"] = m_rotationAngle;
    return json;
}

void Shape::fromJson(const QJsonObject &json)
{
    if (json.contains("penColor"))
        m_penColor = QColor(json["penColor"].toString());
    
    if (json.contains("penWidth"))
        m_penWidth = json["penWidth"].toInt();
    
    if (json.contains("fillColor"))
        m_fillColor = QColor(json["fillColor"].toString());
    
    if (json.contains("penStyle"))
        m_penStyle = static_cast<Qt::PenStyle>(json["penStyle"].toInt());
    if(json.contains("rotationAngle"))
        m_rotationAngle = json["rotationAngle"].toDouble();
}

bool Shape::isSelected() const{
    return m_selected;
}

void Shape::setSelected(bool selected){
    if(m_selected != selected){
        m_selected = selected;
        emit shapeChanged();
    }
}

bool Shape::isAnimating() const{
    return m_animating;
}

void Shape::setAnimating(bool animating){
    if(m_animating != animating){
        m_animating = animating;
        emit shapeChanged();
    }
}

