#ifndef ROUNDEDWIDGET_H
#define ROUNDEDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QHBoxLayout>  // 使用 QHBoxLayout
#include <QLinearGradient>

class RoundedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RoundedWidget(QWidget *parent = nullptr)
        : QWidget(parent), m_radius(15)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        m_layout = new QHBoxLayout(this);  // 使用 QHBoxLayout
        m_layout->setContentsMargins(15, 15, 15, 15);
        setLayout(m_layout);

        setFixedHeight(50);  // 设置固定高度
    }

    void setRadius(int radius) { m_radius = radius; update(); }
    QHBoxLayout* layoutContainer() { return m_layout; }  // 返回 QHBoxLayout

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
        gradient.setColorAt(0.0, QColor("#a0cfff"));
        gradient.setColorAt(1.0, QColor("#e0f4ff"));

        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), m_radius, m_radius);
    }

private:
    int m_radius;
    QHBoxLayout *m_layout;  // 使用 QHBoxLayout
};

#endif // ROUNDEDWIDGET_H
