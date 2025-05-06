#include "../include/PropertiesDialog.h"
#include <QFormLayout>

PropertiesDialog::PropertiesDialog(QWidget* parent) : QDialog(parent) {
    QFormLayout* layout = new QFormLayout(this);
    
    angleSpinBox = new QDoubleSpinBox();
    angleSpinBox->setRange(0, 360);
    angleSpinBox->setSuffix("°");
    layout->addRow("Rotation angle:", angleSpinBox);
    
    widthSpinBox = new QSpinBox();
    widthSpinBox->setRange(1, 20);
    widthSpinBox->setSuffix(" px");
    layout->addRow("Line width:", widthSpinBox);
    
    penColorButton = new QPushButton("Choose...");
    connect(penColorButton, &QPushButton::clicked, [this](){
        QColor color = QColorDialog::getColor(m_penColor, this);
        if(color.isValid()) {
            m_penColor = color;
            penColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        }
    });
    layout->addRow("Line color:", penColorButton);
    
    fillColorButton = new QPushButton("Choose...");
    connect(fillColorButton, &QPushButton::clicked, [this](){
        QColor color = QColorDialog::getColor(m_fillColor, this);
        if(color.isValid()) {
            m_fillColor = color;
            fillColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        }
    });
    layout->addRow("Fill color:", fillColorButton);
    
    QPushButton* okButton = new QPushButton("OK");
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addRow(okButton);
}
