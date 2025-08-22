#include "AnimatedCheckBox.h"
#include <QStyleOptionButton>
#include <QStyle>
#include <QPixmap>

AnimatedCheckBox::AnimatedCheckBox(QWidget *parent) : QCheckBox(parent)
{
    setText("");
    setFixedSize(30, 30);

    // 仅保留边框样式，图标用自绘
    setStyleSheet(R"(
    QCheckBox {
        background-color: #d3d3d3;
        border: 3px solid #808080;
        border-radius: 5px;
        padding: 0;
    }

    QCheckBox::indicator {
        width: 0px;         /* 隐藏勾选框 */
        height: 0px;
        border: none;
        background: none;
    }
)");

    animation = new QPropertyAnimation(this, "scaleFactor");
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve::OutQuad);

    connect(this, &QCheckBox::toggled, this, &AnimatedCheckBox::onToggled);
}

qreal AnimatedCheckBox::scaleFactor() const { return m_scaleFactor; }
void AnimatedCheckBox::setScaleFactor(qreal factor) { m_scaleFactor = factor; update(); }

void AnimatedCheckBox::paintEvent(QPaintEvent *event)
{
    QCheckBox::paintEvent(event);

    if (!isChecked()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPixmap pixmap(":/images/images/checked_icon.png");

    int w = pixmap.width() * m_scaleFactor;
    int h = pixmap.height() * m_scaleFactor;

    int x = (width() - w) / 2;
    int y = (height() - h) / 2;

    painter.drawPixmap(x, y, w, h, pixmap);
}

void AnimatedCheckBox::onToggled(bool checked)
{
    animation->stop();
    if (checked)
    {
        animation->setStartValue(0.0);
        animation->setEndValue(1.0);
    }
    else
    {
        animation->setStartValue(1.0);
        animation->setEndValue(0.0);
    }
    animation->start();
}
