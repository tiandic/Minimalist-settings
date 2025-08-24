#ifndef ROUNDEDWIDGET_H
#define ROUNDEDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QHBoxLayout>  // 使用 QHBoxLayout
#include <QLinearGradient>
#include <QEvent>

class RoundedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RoundedWidget(QWidget *parent = nullptr)
        : QWidget(parent), m_radius(15), m_hovered(false)
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

        // 设置透明背景
        painter.setBrush(Qt::transparent);  // 设置为透明背景
        painter.setPen(Qt::NoPen);  // 不绘制边框

        // 绘制圆角矩形
        painter.drawRoundedRect(rect(), m_radius, m_radius);
        if (m_hovered) {
            painter.setPen(QPen(Qt::white, 2));  // 设置白色边框
            painter.drawRoundedRect(rect(), m_radius, m_radius);
        }
    }
    void enterEvent(QEnterEvent *event) override {
        QWidget::enterEvent(event);
        m_hovered = true;
        update();  // 更新界面，显示边框
    }

    void leaveEvent(QEvent *event) override {
        QWidget::leaveEvent(event);
        m_hovered = false;
        update();  // 更新界面，隐藏边框
    }

private:
    int m_radius;
    QHBoxLayout *m_layout;  // 使用 QHBoxLayout
    bool m_hovered;  // 标记是否悬停
};

#endif // ROUNDEDWIDGET_H
