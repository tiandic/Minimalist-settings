#ifndef ANIMATEDCHECKBOX_H
#define ANIMATEDCHECKBOX_H
#pragma once
#include <QCheckBox>
#include <QPropertyAnimation>
#include <QPainter>

class AnimatedCheckBox : public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY(qreal scaleFactor READ scaleFactor WRITE setScaleFactor)

public:
    AnimatedCheckBox(QWidget *parent = nullptr);

    qreal scaleFactor() const;
    void setScaleFactor(qreal factor);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onToggled(bool checked);

private:
    QPropertyAnimation *animation;
    qreal m_scaleFactor = 1.0;
};

#endif // ANIMATEDCHECKBOX_H
