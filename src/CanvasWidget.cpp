#include "../include/CanvasWidget.h"
#include "../include/shapes/LineShape.h"
#include "../include/shapes/FreehandShape.h"
#include "../include/shapes/RectangleShape.h"
#include "../include/shapes/EllipseShape.h"
#include "../include/shapes/PolygonShape.h"
#include "../include/shapes/RegularPolygonShape.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QMenu>

CanvasWidget::CanvasWidget(QWidget* parent) : QWidget(parent){
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(true);
    setMinimumSize(400, 300);
}

QColor CanvasWidget::penColor(){
    return m_penColor;
}

QColor CanvasWidget::fillColor(){
    return m_fillColor;
}

int CanvasWidget::penWidth(){
    return m_penWidth;
}

void CanvasWidget::setCurrentShapeType(const QString& shapeType){
    m_currentShapeType = shapeType;
}

void CanvasWidget::setPenColor(const QColor& color){
    m_penColor = color;
    if(m_selectedShape){
        m_selectedShape->setPenColor(color);
        update();
    }
}

void CanvasWidget::setPenWidth(int width){
    m_penWidth = width;
    if(m_selectedShape){
        m_selectedShape->setPenWidth(width);
        update();
    }
}

void CanvasWidget::setFillColor(const QColor& color){
    m_fillColor = color;
    if(m_selectedShape){
        m_selectedShape->setFillColor(color);
        update();
    }
}

void CanvasWidget::paintEvent(QPaintEvent* event){
    Q_UNUSED(event);


    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);


    for(Shape* shape : m_shapes){
        shape->draw(&painter);
    }

    if(m_currentShape && m_isDrawing){
        m_currentShape->draw(&painter);
    }
}


void CanvasWidget::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton){
        m_lastPoint = event->pos();
        if(!m_selectedShape){
            m_dragging = false;
            if(m_currentShapeType == "Freehand"){
                m_currentShape = createShape(m_currentShapeType);
                if(FreehandShape* freehand = qobject_cast<FreehandShape*>(m_currentShape)){
                    freehand->addPoint(m_lastPoint);
                }
                m_isDrawing = true;
            }
            else if(m_currentShapeType == "Polygon"){
                if(!m_currentShape){
                    m_currentShape = createShape(m_currentShapeType);
                    m_isDrawing = true;
                }
                if (PolygonShape* polygon = qobject_cast<PolygonShape*>(m_currentShape)) {
                    polygon->addPoint(m_lastPoint);
                }
                update();
            }
            else {
                m_currentShape = createShape(m_currentShapeType);
                m_isDrawing = true;
            }
        }
        else{
        }
        m_isModified = true;
        emit fileModified(true);
    }
    else if (event->button() == Qt::RightButton) {
        selectShape(event->pos());

        qDebug() << "MOUSE PRESS SELECTED";
        if(m_selectedShape != nullptr){
            QRect rect = m_selectedShape->boundingRect();
                if(rect.contains(m_lastPoint)){
                    if(!m_dragging && !m_rotating && !m_resizing)
                        m_dragging = true;
                    else{
                        if(m_dragging){
                            m_dragging = false;
                            m_resizing = true;
                        }
                        else if(m_resizing){
                            m_resizing = false;
                            m_rotating = true;
                        }
                        else{
                            m_rotating = false;
                            m_dragging = true;
                        }
                    }
                }
                else 
                    m_selectedShape = nullptr;
                return;
        }
    }
    
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event){

    QPoint m_currentPoint = event->pos();
    m_scale = 1;

    if ((event->buttons() & Qt::LeftButton)){
        if(m_isDrawing && m_currentShape) {
            if (m_currentShapeType == "Freehand") {
                if (FreehandShape* freehand = qobject_cast<FreehandShape*>(m_currentShape)) {
                    freehand->addPoint(event->pos());
                }
            } else {
                if(m_currentShapeType != "Polygon"){
                    m_currentShape->update(event->pos());
                }
            }
            update();
        }
        else{
            if(m_selectedShape){ 
                if (m_dragging){
                    m_selectedShape->move((m_currentPoint - m_lastPoint));
                    m_lastPoint = m_currentPoint;
                }
                if (m_resizing) {
                    m_scale = m_currentPoint.x() - m_lastPoint.x();
                }
                if (m_rotating){
                    QRect rect = m_selectedShape->boundingRect();
                    QPointF center = rect.center();

                    QPointF vec = m_currentPoint - center;
                    qreal angle = (std::atan2(vec.y(), vec.x()) - std::atan2(center.y(), center.x())) * 180 / M_PI;

                    m_selectedShape->rotate(angle);
                }
                update();
            }
        }
    }
}
    
void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton) {
        if(m_isDrawing && m_currentShape) {
            if (m_currentShapeType == "Freehand") {
                m_shapes.append(m_currentShape);
                m_currentShape = nullptr;
            }
            else if (m_currentShapeType != "Polygon") {
                m_shapes.append(m_currentShape);
                m_currentShape = nullptr;
            }
            m_isDrawing = false;
            update();
        }
        if(m_selectedShape){ 
            if (m_resizing) {
                m_selectedShape->scale(m_scale / 70);
            }
            update();
        }
    }

}

void CanvasWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && 
        m_currentShapeType == "Polygon" && 
        m_currentShape)
    {
        PolygonShape* polygon = qobject_cast<PolygonShape*>(m_currentShape);
        if (polygon) {
            polygon->closePolygon();
            m_shapes.append(m_currentShape);
            m_currentShape = nullptr;
            m_isDrawing = false;
            update();
        }
    }
}

void CanvasWidget::contextMenuEvent(QContextMenuEvent* event){
    selectShape(event->pos());
    if(!m_currentShape)
        return;
    
    QMenu menu(this);
    QAction* deleteAction = menu.addAction("Delete");
    QAction* animateAction = menu.addAction("Animate");
    QAction* bringToFrontAction = menu.addAction("Bring to front");
    QAction* sendToBackAction = menu.addAction("Send to back");

    QAction* selectedAction = menu.exec(event->globalPos());
    if(selectedAction == deleteAction){
        deleteSelectedShape();
    }
    else{
        if(selectedAction == animateAction){
            startAnimation();
        }
        else{
            if(selectedAction == bringToFrontAction){
                bringToFront();
            }
            else{
                if(selectedAction == sendToBackAction){
                    sendToBack();
                }
            }
        }
    }
}

Shape* CanvasWidget::createShape(const QString& shapeType){
    Shape* shape = nullptr;


    if(shapeType == "Line"){
        shape = new LineShape(m_lastPoint, m_lastPoint, this);
    }
    else{
        if(shapeType == "Freehand"){
            shape = new FreehandShape(this);
        }
        else{
            if(shapeType == "Rectangle"){
                shape = new RectangleShape(m_lastPoint, m_lastPoint, this);
            }
            else{
                if(shapeType == "Ellipse"){
                    shape = new EllipseShape(m_lastPoint, 0, 0, this);
                }
                else{
                    if(shapeType == "Polygon"){
                        shape = new PolygonShape(this);
                    }
                    else{
                        if(shapeType == "RegularPolygon"){
                            shape = new RegularPolygonShape(m_lastPoint, 0, 5, this);
                        }
                    }
                }
            }
        }
    }
    
    if(shape){
        shape->setPenColor(m_penColor);
        shape->setPenWidth(m_penWidth);
        shape->setFillColor(m_fillColor);
    }

    return shape;
}

void CanvasWidget::selectShape(const QPoint& point){
    for(Shape* shape : m_shapes){
        shape->setSelected(false);
    }
    m_selectedShape = nullptr;
    /*m_resizing = false;
    m_dragging = false;
    m_rotating = false;*/

    m_currentShape = nullptr;
    for(int i = m_shapes.size() - 1; i >= 0; --i){
        if(m_shapes[i]->contains(point)){
            m_shapes[i]->setSelected(true);
            m_selectedShape = m_shapes[i];
            emit shapeSelected(m_selectedShape->name() + " selected");
            break;
        }
    }

    update();
}

bool CanvasWidget::saveToFile(const QString& filename){
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return false;

    qDebug() << "File created";
    QJsonObject root;
    root["version"] = 1;
    root["penColor"] = m_penColor.name();
    root["penWidth"] = m_penWidth;

    QJsonArray shapeArray;
    qDebug() << "Before shapes";
    for (const auto& shape : m_shapes) {
        qDebug() << "Shape " << shape->name();
        QJsonObject obj = shape->toJson();
        obj["type"] = shape->name();
        shapeArray.append(obj);
        qDebug() << "Shape created";
    }

    root["shapes"] = shapeArray;

    qDebug() << "Before doc";

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();

    fileModified(false);
    return true;
}

bool CanvasWidget::loadFromFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return false;

    QJsonObject root = doc.object();
    if (!root.contains("shapes")) return false;

    m_shapes.clear();
    QJsonArray shapeArray = root["shapes"].toArray();
    for (const QJsonValue &val : shapeArray) {
        QJsonObject obj = val.toObject();
        QString type = obj["type"].toString();
        Shape* shape;

        if (type == "Line") shape = new LineShape();
        else if (type == "Ellipse") shape = new EllipseShape();
        else if (type == "Rectangle") shape = new RectangleShape();
        else if (type == "Freehand") shape = new FreehandShape();
        else if (type == "Polygon") shape = new PolygonShape();
        else if (type == "Regular polygon") shape = new RegularPolygonShape();

        if (shape) {
            shape->fromJson(obj);
            m_shapes.append(shape);
        }
    }

    fileModified(false);
    update();
    return true;
}

void CanvasWidget::clearCanvas(){
    qDeleteAll(m_shapes);
    m_shapes.clear();
    m_currentShape = nullptr;
    m_isModified = false;
    emit fileModified(false);
    update();
}

void CanvasWidget::deleteSelectedShape(){
    if(!m_selectedShape)
        return;

    m_shapes.removeOne(m_selectedShape);
    delete m_selectedShape;
    m_selectedShape = nullptr;
    m_isModified = true;
    emit fileModified(true);
    update();
}


void CanvasWidget::bringToFront()
{
    if (!m_selectedShape) return;
    
    m_shapes.removeOne(m_selectedShape);
    m_shapes.append(m_selectedShape);
    m_isModified = true;
    emit fileModified(true);
    update();
}

void CanvasWidget::sendToBack()
{
    if (!m_selectedShape) return;
    
    m_shapes.removeOne(m_selectedShape);
    m_shapes.prepend(m_selectedShape);
    m_isModified = true;
    emit fileModified(true);
    update();
}

void CanvasWidget::startAnimation(){
    if(!m_selectedShape)
        return;
    /*
        Animation
    */
    QMessageBox::information(this, "Animation", "Animation for " + m_selectedShape->name());
}

void CanvasWidget::stopAnimation(){
    /*
        Stop
    */
}

void CanvasWidget::resizeEvent(QResizeEvent *event)
{
    if (event->oldSize().width() > 0 && event->oldSize().height() > 0) {
        double xFactor = width() / double(event->oldSize().width());
        double yFactor = height() / double(event->oldSize().height());
        scaleShapes(qMin(xFactor, yFactor));
    }
    QWidget::resizeEvent(event);
}

void CanvasWidget::scaleShapes(double factor)
{
    for (Shape *shape : m_shapes) {
        shape->scale(factor);
    }
    update();
}