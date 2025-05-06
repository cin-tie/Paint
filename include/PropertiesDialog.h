#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QColorDialog>
#include <QPushButton>

class PropertiesDialog : public QDialog {
    Q_OBJECT
public:
    PropertiesDialog(QWidget* parent = nullptr);
    
    void setAngle(double angle);
    void setPenColor(const QColor& color);
    void setPenWidth(int width);
    void setFillColor(const QColor& color);
    
    double angle() const;
    QColor penColor() const;
    int penWidth() const;
    QColor fillColor() const;

private:
    QDoubleSpinBox* angleSpinBox;
    QSpinBox* widthSpinBox;
    QPushButton* penColorButton;
    QPushButton* fillColorButton;
    QColor m_penColor;
    QColor m_fillColor;
};

#endif