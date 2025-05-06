#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include "./shapes/Shape.h"
#include <QWidget>

class CanvasWidget : public QWidget{

    Q_OBJECT

    public:
        explicit CanvasWidget(QWidget* parent = nullptr);

        QColor penColor();
        QColor fillColor();
        int penWidth();
        void setCurrentShapeType(const QString& shapeType);
        void setPenColor(const QColor& color);
        void setPenWidth(int width);
        void setFillColor(const QColor& color);
        
        bool saveToFile(const QString& filename);
        bool loadFromFile(const QString& filename);
        
        void clearCanvas();
        void deleteSelectedShape();
        void bringToFront();
        void sendToBack();
        
        void startAnimation();
        void stopAnimation();

    signals:
        void shapeSelected(const QString& shapeInfo);
        void fileModified(bool modified);

    protected:
        void paintEvent(QPaintEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        void contextMenuEvent(QContextMenuEvent* event) override;

    private:
        QList<Shape*> m_shapes;
        Shape* m_currentShape = nullptr;
        QString m_currentShapeType;
        QPoint m_lastPoint;
        bool m_isDrawing = false;
        bool m_isModified = false;
        QColor m_penColor = Qt::black;
        int m_penWidth = 1;
        QColor m_fillColor = Qt::transparent;

        Shape* createShape(const QString& shapeType);
        void selectShape(const QPoint& point);
        void scaleShapes(double factor);
        void updateSelection();
};

#endif