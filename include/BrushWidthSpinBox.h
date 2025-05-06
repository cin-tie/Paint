#ifndef BRUSHWIDTHSPINBOX_H
#define BRUSHWIDTHSPINBOX_H

#include <QSpinBox>

class BrushWidthSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit BrushWidthSpinBox(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif