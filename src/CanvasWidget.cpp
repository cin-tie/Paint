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
    if(m_currentShape){
        m_currentShape->setPenColor(color);
        update();
    }
}

void CanvasWidget::setPenWidth(int width){
    m_penWidth = width;
    if(m_currentShape){
        m_currentShape->setPenWidth(width);
        update();
    }
}

void CanvasWidget::setFillColor(const QColor& color){
    m_fillColor = color;
    if(m_currentShape){
        m_currentShape->setFillColor(color);
        update();
    }
}

void CanvasWidget::paintEvent(QPaintEvent* event){
    Q_UNUSED(event);

    qDebug() << "Paint event";

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);


    qDebug() << m_shapes.size();
    for(Shape* shape : m_shapes){
        qDebug() << "Paint event shape";
        shape->draw(&painter);
    }

    if(m_currentShape && m_isDrawing){
        m_currentShape->draw(&painter);
    }
}


void CanvasWidget::mousePressEvent(QMouseEvent *event){

    qDebug() << "Mouse press" << m_currentShapeType;

    if(event->button() == Qt::LeftButton){
        m_lastPoint = event->pos();
        
        if(m_currentShapeType == "Freehand"){
            m_currentShape = createShape(m_currentShapeType);
            if(FreehandShape* freehand = qobject_cast<FreehandShape*>(m_currentShape)){
                freehand->addPoint(m_lastPoint);
            }
            m_isDrawing = true;
        }
        else{
            if(m_currentShapeType == "Polygon"){
                if(!m_currentShape){
                    m_currentShape = createShape(m_currentShapeType);
                    if (PolygonShape* polygon = qobject_cast<PolygonShape*>(m_currentShape)) {
                        polygon->addPoint(m_lastPoint);
                    }
                    m_isDrawing = true;
                } 
                else{
                    if (PolygonShape* polygon = qobject_cast<PolygonShape*>(m_currentShape)) {
                        polygon->addPoint(m_lastPoint);
                    }
                    update();
                }
            }
            else {
                m_currentShape = createShape(m_currentShapeType);
                qDebug() << "Shape created" << (m_currentShape == nullptr);
                m_isDrawing = true;
            }
        }
        
        m_isModified = true;
        emit fileModified(true);
    }
    else if (event->button() == Qt::RightButton) {
        selectShape(event->pos());
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event){

    //qDebug() << "Mouse move";

    if ((event->buttons() & Qt::LeftButton) && m_isDrawing && m_currentShape) {
        if (m_currentShapeType == "Freehand") {
            if (FreehandShape* freehand = qobject_cast<FreehandShape*>(m_currentShape)) {
                freehand->addPoint(event->pos());
            }
        } else {
            m_currentShape->update(event->pos());
        }
        update();
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{

    qDebug() << "Mouse release";

    qDebug() << m_isDrawing << (m_currentShape == nullptr);

    if (event->button() == Qt::LeftButton && m_isDrawing && m_currentShape) {
        if (m_currentShapeType != "Freehand" && m_currentShapeType != "Polygon") {
            m_shapes.append(m_currentShape);
            qDebug() << "Shape appended";
            qDebug() << m_shapes.size();
            m_currentShape = nullptr;
        }
        m_isDrawing = false;
        update();
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

    qDebug() << "Shape creating" << shapeType << (shape == nullptr);

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
    
    qDebug() << "Shape created" << shapeType << (shape == nullptr);
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

    m_currentShape = nullptr;
    for(int i = m_shapes.size() - 1; i >= 0; --i){
        if(m_shapes[i]->contains(point)){
            m_shapes[i]->setSelected(true);
            m_currentShape = m_shapes[i];
            emit shapeSelected(m_currentShape->name() + " selected");
            break;
        }
    }

    update();
}

bool CanvasWidget::saveToFile(const QString& filename){
    QJsonArray shapesArray;
    for(Shape* shape : m_shapes){
        shapesArray.append(shape->toJson());
    }

    QJsonDocument doc(shapesArray);
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)){
        return false;
    }

    file.write(doc.toJson());
    file.close();
    m_isModified = false;
    emit fileModified(false);
    return true;
}

bool CanvasWidget::loadFromFile(const QString& filename){
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isArray()){
        return false;
    }

    clearCanvas();

    QJsonArray shapesArray = doc.array();
    for(const QJsonValue& value : shapesArray){
        QJsonObject shapeObject = value.toObject();
        if(shapeObject.contains("type")){
            QString type = shapeObject["type"].toString();
            Shape* shape = createShape(type);
            if(shape){
                shape->fromJson(shapeObject);   
                m_shapes.append(shape);
            }
        }
    }

    m_isModified = false;
    emit fileModified(false);
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
    if(!m_currentShape)
        return;

    m_shapes.removeOne(m_currentShape);
    delete m_currentShape;
    m_currentShape = nullptr;
    m_isModified = true;
    emit fileModified(true);
    update();
}


void CanvasWidget::bringToFront()
{
    if (!m_currentShape) return;
    
    m_shapes.removeOne(m_currentShape);
    m_shapes.append(m_currentShape);
    m_isModified = true;
    emit fileModified(true);
    update();
}

void CanvasWidget::sendToBack()
{
    if (!m_currentShape) return;
    
    m_shapes.removeOne(m_currentShape);
    m_shapes.prepend(m_currentShape);
    m_isModified = true;
    emit fileModified(true);
    update();
}

void CanvasWidget::startAnimation(){
    if(!m_currentShape)
        return;
    /*
        Animation
    */
    QMessageBox::information(this, "Animation", "Animation for " + m_currentShape->name());
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