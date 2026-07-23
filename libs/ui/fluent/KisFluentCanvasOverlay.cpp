/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 KritaFluent Contributors
 */

#include "KisFluentCanvasOverlay.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QtMath>
#include <QPaintEvent>

KisFluentCanvasOverlay::KisFluentCanvasOverlay(QWidget *parent)
    : QWidget(parent)
    , m_brushSize(10.0f)
    , m_brushOpacity(1.0f)
    , m_showBrushCursor(false)
    , m_flashOpacity(0.0)
    , m_showEyedropper(false)
    , m_showMorph(false)
    , m_morphProgress(0.0)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);

    m_flashAnimation = new QPropertyAnimation(this, "flashOpacity", this);
    m_flashAnimation->setDuration(300);
    m_flashAnimation->setEasingCurve(QEasingCurve::OutQuad);

    m_morphAnimation = new QPropertyAnimation(this, "morphProgress", this);
    m_morphAnimation->setDuration(400);
    m_morphAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    
    connect(m_morphAnimation, &QPropertyAnimation::finished, this, [this]() {
        m_showMorph = false;
        update();
    });
}

KisFluentCanvasOverlay::~KisFluentCanvasOverlay()
{
}

void KisFluentCanvasOverlay::updateCursor(QPointF pos, float size, float opacity)
{
    m_cursorPosition = pos;
    m_brushSize = size;
    m_brushOpacity = opacity;
    update();
}

void KisFluentCanvasOverlay::showUndoFlash()
{
    m_flashColor = QColor(255, 255, 255);
    m_flashAnimation->stop();
    m_flashAnimation->setStartValue(0.3);
    m_flashAnimation->setEndValue(0.0);
    m_flashAnimation->start();
}

void KisFluentCanvasOverlay::showRedoFlash()
{
    m_flashColor = QColor(0, 120, 215); // Fluent Blue
    m_flashAnimation->stop();
    m_flashAnimation->setStartValue(0.2);
    m_flashAnimation->setEndValue(0.0);
    m_flashAnimation->start();
}

void KisFluentCanvasOverlay::showEyedropper(QPointF pos, QColor sampledColor, QColor currentColor)
{
    m_showEyedropper = true;
    m_eyedropperPos = pos;
    m_eyedropperSampledColor = sampledColor;
    m_eyedropperCurrentColor = currentColor;
    update();
}

void KisFluentCanvasOverlay::hideEyedropper()
{
    m_showEyedropper = false;
    update();
}

void KisFluentCanvasOverlay::showQuickShapePreview(QPainterPath original, QPainterPath snapped)
{
    m_showMorph = true;
    m_morphOriginal = original;
    m_morphSnapped = snapped;
    m_morphAnimation->stop();
    m_morphAnimation->setStartValue(0.0);
    m_morphAnimation->setEndValue(1.0);
    m_morphAnimation->start();
}

qreal KisFluentCanvasOverlay::flashOpacity() const
{
    return m_flashOpacity;
}

void KisFluentCanvasOverlay::setFlashOpacity(qreal opacity)
{
    m_flashOpacity = opacity;
    update();
}

qreal KisFluentCanvasOverlay::morphProgress() const
{
    return m_morphProgress;
}

void KisFluentCanvasOverlay::setMorphProgress(qreal progress)
{
    m_morphProgress = progress;
    update();
}

void KisFluentCanvasOverlay::setShowBrushCursor(bool show)
{
    m_showBrushCursor = show;
    update();
}

bool KisFluentCanvasOverlay::showBrushCursor() const
{
    return m_showBrushCursor;
}

void KisFluentCanvasOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. Draw Flash Overlay
    if (m_flashOpacity > 0.0) {
        QColor fColor = m_flashColor;
        fColor.setAlphaF(m_flashOpacity);
        painter.fillRect(rect(), fColor);
    }

    // 2. Draw QuickShape Morphing
    if (m_showMorph) {
        // Interpolate paths (simplified as drawing both with varying opacities for now,
        // true path morphing requires more complex path interpolation)
        QPen pen(QColor(0, 120, 215), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        
        QColor origColor(0, 120, 215);
        origColor.setAlphaF(1.0 - m_morphProgress);
        pen.setColor(origColor);
        painter.setPen(pen);
        painter.drawPath(m_morphOriginal);

        QColor snapColor(0, 120, 215);
        snapColor.setAlphaF(m_morphProgress);
        pen.setColor(snapColor);
        painter.setPen(pen);
        painter.drawPath(m_morphSnapped);
    }

    // 3. Draw Brush Cursor
    if (m_showBrushCursor) {
        QColor cursorColor(128, 128, 128);
        cursorColor.setAlphaF(m_brushOpacity);
        
        painter.setPen(QPen(QColor(0, 0, 0, 128), 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(m_cursorPosition, m_brushSize / 2.0, m_brushSize / 2.0);
        
        // Inner white ring for contrast
        painter.setPen(QPen(QColor(255, 255, 255, 128), 1));
        painter.drawEllipse(m_cursorPosition, (m_brushSize / 2.0) - 1.0, (m_brushSize / 2.0) - 1.0);
    }

    // 4. Draw Eyedropper Loupe
    if (m_showEyedropper) {
        const qreal loupeRadius = 60.0;
        const qreal offset = 80.0; // Offset above finger
        QPointF center = m_eyedropperPos - QPointF(0, offset);

        // Clip region for loupe
        QPainterPath clipPath;
        clipPath.addEllipse(center, loupeRadius, loupeRadius);
        painter.setClipPath(clipPath);

        // Draw split colors
        painter.fillRect(QRectF(center.x() - loupeRadius, center.y() - loupeRadius, loupeRadius * 2, loupeRadius), m_eyedropperSampledColor);
        painter.fillRect(QRectF(center.x() - loupeRadius, center.y(), loupeRadius * 2, loupeRadius), m_eyedropperCurrentColor);

        painter.setClipping(false);

        // Draw loupe border
        QPen borderPen(QColor(200, 200, 200), 4);
        painter.setPen(borderPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(center, loupeRadius, loupeRadius);

        // Draw center dot
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.drawEllipse(center, 3, 3);
    }
}
