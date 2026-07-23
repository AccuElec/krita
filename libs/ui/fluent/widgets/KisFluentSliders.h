/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 KritaFluent Contributors
 */

#ifndef KIS_FLUENT_SLIDERS_H
#define KIS_FLUENT_SLIDERS_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>

/**
 * @brief Touch-optimized custom sliders for Brush Size and Opacity.
 */
class KisFluentSliders : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal brushSize READ brushSize WRITE setBrushSize NOTIFY brushSizeChanged)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(qreal collapseProgress READ collapseProgress WRITE setCollapseProgress)

public:
    explicit KisFluentSliders(QWidget *parent = nullptr);
    ~KisFluentSliders() override;

    /**
     * @brief Sets whether the sliders are on the left or right edge.
     * @param dir Qt::LeftToRight for left edge, Qt::RightToLeft for right edge.
     */
    void setHandedness(Qt::LayoutDirection dir);

    /**
     * @brief Enables auto-collapsing to a thin line after inactivity.
     */
    void setAutoCollapse(bool autoCollapse);

    qreal brushSize() const;
    void setBrushSize(qreal size);

    qreal opacity() const;
    void setOpacity(qreal opacity);

    qreal collapseProgress() const;
    void setCollapseProgress(qreal progress);

signals:
    void brushSizeChanged(qreal size);
    void opacityChanged(qreal opacity);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void enterEvent(QEvent *event) override;

private:
    enum SliderType { None, Size, Opacity };

    qreal m_brushSize;
    qreal m_opacity;
    Qt::LayoutDirection m_handedness;
    bool m_autoCollapse;
    qreal m_collapseProgress; // 0.0 = fully expanded, 1.0 = fully collapsed
    
    SliderType m_activeSlider;
    bool m_isDragging;
    QPoint m_dragStartPos;
    
    QTimer *m_collapseTimer;
    QPropertyAnimation *m_collapseAnimation;

    QRectF sizeSliderRect() const;
    QRectF opacitySliderRect() const;
    void updateValueFromPos(const QPoint &pos);
    void resetCollapseTimer();
};

#endif // KIS_FLUENT_SLIDERS_H
