/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 KritaFluent Contributors
 */

#include "KisFluentSliders.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>
#include <QStyleOption>

KisFluentSliders::KisFluentSliders(QWidget *parent)
    : QWidget(parent)
    , m_brushSize(10.0)
    , m_opacity(1.0)
    , m_handedness(Qt::LeftToRight)
    , m_autoCollapse(true)
    , m_collapseProgress(0.0)
    , m_activeSlider(None)
    , m_isDragging(false)
{
    setFixedWidth(56);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    m_collapseTimer = new QTimer(this);
    m_collapseTimer->setInterval(3000);
    m_collapseTimer->setSingleShot(true);
    
    m_collapseAnimation = new QPropertyAnimation(this, "collapseProgress", this);
    m_collapseAnimation->setDuration(300);
    m_collapseAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(m_collapseTimer, &QTimer::timeout, this, [this]() {
        if (m_autoCollapse && !m_isDragging && !underMouse()) {
            m_collapseAnimation->setStartValue(m_collapseProgress);
            m_collapseAnimation->setEndValue(1.0);
            m_collapseAnimation->start();
        }
    });

    m_collapseTimer->start();
}

KisFluentSliders::~KisFluentSliders()
{
}

void KisFluentSliders::setHandedness(Qt::LayoutDirection dir)
{
    m_handedness = dir;
    update();
}

void KisFluentSliders::setAutoCollapse(bool autoCollapse)
{
    m_autoCollapse = autoCollapse;
    if (m_autoCollapse) {
        resetCollapseTimer();
    } else {
        m_collapseTimer->stop();
        m_collapseAnimation->setStartValue(m_collapseProgress);
        m_collapseAnimation->setEndValue(0.0);
        m_collapseAnimation->start();
    }
}

qreal KisFluentSliders::brushSize() const
{
    return m_brushSize;
}

void KisFluentSliders::setBrushSize(qreal size)
{
    m_brushSize = qBound(1.0, size, 1000.0);
    update();
    emit brushSizeChanged(m_brushSize);
}

qreal KisFluentSliders::opacity() const
{
    return m_opacity;
}

void KisFluentSliders::setOpacity(qreal opacity)
{
    m_opacity = qBound(0.0, opacity, 1.0);
    update();
    emit opacityChanged(m_opacity);
}

qreal KisFluentSliders::collapseProgress() const
{
    return m_collapseProgress;
}

void KisFluentSliders::setCollapseProgress(qreal progress)
{
    m_collapseProgress = progress;
    update();
}

QRectF KisFluentSliders::sizeSliderRect() const
{
    int w = width();
    int h = height() / 2 - 20;
    return QRectF(w / 2 - 4, 20, 8, h);
}

QRectF KisFluentSliders::opacitySliderRect() const
{
    int w = width();
    int halfH = height() / 2;
    int h = halfH - 40;
    return QRectF(w / 2 - 4, halfH + 20, 8, h);
}

void KisFluentSliders::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    qreal currentWidth = 8.0 * (1.0 - m_collapseProgress) + 2.0 * m_collapseProgress;
    qreal handleSize = 44.0 * (1.0 - m_collapseProgress);
    if (handleSize < 1.0) return; // Completely collapsed visually

    QColor trackColor(0, 0, 0, 100);
    QColor activeColor(0, 120, 215);

    // Draw Size Slider
    QRectF sRect = sizeSliderRect();
    sRect.setLeft(width() / 2 - currentWidth / 2);
    sRect.setWidth(currentWidth);
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(trackColor);
    painter.drawRoundedRect(sRect, currentWidth/2, currentWidth/2);

    // Size handle
    qreal sVal = (m_brushSize - 1.0) / 999.0; // Normalized 0-1
    qreal sY = sRect.bottom() - sVal * sRect.height();
    
    painter.setBrush(Qt::white);
    painter.setPen(QPen(trackColor, 1));
    painter.drawEllipse(QPointF(width()/2, sY), handleSize/2, handleSize/2);

    // Size Preview inside handle
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    qreal previewSize = qMax(2.0, (handleSize/2 - 4) * sVal);
    painter.drawEllipse(QPointF(width()/2, sY), previewSize, previewSize);

    // Draw Opacity Slider
    QRectF oRect = opacitySliderRect();
    oRect.setLeft(width() / 2 - currentWidth / 2);
    oRect.setWidth(currentWidth);

    painter.setBrush(trackColor);
    painter.drawRoundedRect(oRect, currentWidth/2, currentWidth/2);

    // Opacity handle
    qreal oVal = m_opacity;
    qreal oY = oRect.bottom() - oVal * oRect.height();

    painter.setBrush(Qt::white);
    painter.setPen(QPen(trackColor, 1));
    painter.drawEllipse(QPointF(width()/2, oY), handleSize/2, handleSize/2);

    // Opacity preview inside handle (gradient/pie)
    painter.setBrush(QColor(0, 0, 0, 255 * m_opacity));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(width()/2, oY), handleSize/2 - 6, handleSize/2 - 6);

    // Value popup when dragging
    if (m_isDragging) {
        QString text;
        QPointF textPos;
        if (m_activeSlider == Size) {
            text = QString::number(qRound(m_brushSize)) + " px";
            textPos = QPointF(m_handedness == Qt::LeftToRight ? width() + 10 : -60, sY + 5);
        } else if (m_activeSlider == Opacity) {
            text = QString::number(qRound(m_opacity * 100)) + " %";
            textPos = QPointF(m_handedness == Qt::LeftToRight ? width() + 10 : -60, oY + 5);
        }

        if (!text.isEmpty()) {
            painter.setPen(Qt::white);
            QFont f = painter.font();
            f.setBold(true);
            f.setPixelSize(14);
            painter.setFont(f);
            
            // Draw background pill
            QFontMetrics fm(f);
            QRectF bgRect(textPos.x() - 10, textPos.y() - fm.height(), fm.horizontalAdvance(text) + 20, fm.height() + 10);
            painter.setBrush(QColor(0,0,0, 180));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(bgRect, 8, 8);
            
            painter.setPen(Qt::white);
            painter.drawText(bgRect, Qt::AlignCenter, text);
        }
    }
}

void KisFluentSliders::updateValueFromPos(const QPoint &pos)
{
    if (m_activeSlider == Size) {
        QRectF sRect = sizeSliderRect();
        qreal val = 1.0 - qBound(0.0, (pos.y() - sRect.top()) / sRect.height(), 1.0);
        setBrushSize(1.0 + val * 999.0);
    } else if (m_activeSlider == Opacity) {
        QRectF oRect = opacitySliderRect();
        qreal val = 1.0 - qBound(0.0, (pos.y() - oRect.top()) / oRect.height(), 1.0);
        setOpacity(val);
    }
}

void KisFluentSliders::mousePressEvent(QMouseEvent *event)
{
    resetCollapseTimer();
    
    // Check if clicked near size or opacity slider
    QRectF sRect = sizeSliderRect().adjusted(-20, -20, 20, 20);
    QRectF oRect = opacitySliderRect().adjusted(-20, -20, 20, 20);

    if (sRect.contains(event->pos())) {
        m_activeSlider = Size;
        m_isDragging = true;
        updateValueFromPos(event->pos());
    } else if (oRect.contains(event->pos())) {
        m_activeSlider = Opacity;
        m_isDragging = true;
        updateValueFromPos(event->pos());
    }
}

void KisFluentSliders::mouseMoveEvent(QMouseEvent *event)
{
    resetCollapseTimer();
    if (m_isDragging) {
        updateValueFromPos(event->pos());
    }
}

void KisFluentSliders::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_isDragging = false;
    m_activeSlider = None;
    resetCollapseTimer();
    update();
}

void KisFluentSliders::wheelEvent(QWheelEvent *event)
{
    resetCollapseTimer();
    int delta = event->angleDelta().y();
    if (delta == 0) return;

    QRectF sRect = sizeSliderRect().adjusted(-20, -20, 20, 20);
    QRectF oRect = opacitySliderRect().adjusted(-20, -20, 20, 20);
    QPoint pos = event->position().toPoint();

    if (sRect.contains(pos)) {
        setBrushSize(m_brushSize + (delta > 0 ? 5.0 : -5.0));
    } else if (oRect.contains(pos)) {
        setOpacity(m_opacity + (delta > 0 ? 0.05 : -0.05));
    }
}

void KisFluentSliders::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    resetCollapseTimer();
    if (m_collapseProgress > 0.0) {
        m_collapseAnimation->setStartValue(m_collapseProgress);
        m_collapseAnimation->setEndValue(0.0);
        m_collapseAnimation->start();
    }
}

void KisFluentSliders::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    resetCollapseTimer();
}

void KisFluentSliders::resetCollapseTimer()
{
    if (m_autoCollapse) {
        m_collapseTimer->start();
    }
}
