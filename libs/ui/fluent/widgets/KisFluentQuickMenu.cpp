// KisFluentQuickMenu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#include "KisFluentQuickMenu.h"

#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>
#include <QApplication>

KisFluentQuickMenu::KisFluentQuickMenu(QWidget *parent)
    : QWidget(parent),
      m_sectorCount(8),
      m_hoveredSector(-1),
      m_scale(0.0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setFixedSize(240, 240); // 120dp radius

    m_scaleAnimation = new QPropertyAnimation(this, "scale", this);
    m_scaleAnimation->setDuration(200);
    m_scaleAnimation->setEasingCurve(QEasingCurve::OutBack); // Overshoot spring physics
}

KisFluentQuickMenu::~KisFluentQuickMenu() = default;

void KisFluentQuickMenu::setSectorAction(int sector, QAction *action, const QIcon &icon)
{
    if (sector >= 0 && sector < m_sectorCount) {
        m_sectors[sector] = {action, icon};
    }
}

void KisFluentQuickMenu::showAt(QPointF pos)
{
    m_centerPos = pos;
    move(pos.toPoint() - QPoint(width() / 2, height() / 2));
    QWidget::show();

    m_scaleAnimation->setStartValue(0.0);
    m_scaleAnimation->setEndValue(1.0);
    m_scaleAnimation->start();
}

void KisFluentQuickMenu::dismiss()
{
    QWidget::hide();
    emit menuDismissed();
}

int KisFluentQuickMenu::hoveredSector() const
{
    return m_hoveredSector;
}

int KisFluentQuickMenu::sectorCount() const
{
    return m_sectorCount;
}

qreal KisFluentQuickMenu::scale() const
{
    return m_scale;
}

void KisFluentQuickMenu::setScale(qreal scale)
{
    m_scale = scale;
    update();
}

void KisFluentQuickMenu::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPointF center(width() / 2.0, height() / 2.0);
    qreal radius = 120.0 * m_scale;

    if (radius <= 0.0) return;

    painter.translate(center);

    // Background
    painter.setBrush(QColor(28, 28, 30, 220)); // Semi-transparent dark
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(0, 0), radius, radius);

    // Sectors
    qreal angleStep = 360.0 / m_sectorCount;
    painter.setPen(QPen(QColor(255, 255, 255, 30), 1)); // Subtle lines

    for (int i = 0; i < m_sectorCount; ++i) {
        qreal startAngle = i * angleStep - 90 - (angleStep / 2.0);
        
        // Highlight active/hovered sector
        if (i == m_hoveredSector) {
            painter.setBrush(QColor("#5E9CFF")); // Accent color
            painter.drawPie(QRectF(-radius, -radius, radius * 2, radius * 2), -startAngle * 16, -angleStep * 16);
        }

        // Division line
        qreal lineAngle = qDegreesToRadians(startAngle);
        painter.drawLine(QPointF(0, 0), QPointF(radius * qCos(lineAngle), radius * qSin(lineAngle)));

        // Icon
        if (m_sectors.contains(i)) {
            qreal iconAngle = qDegreesToRadians(startAngle + angleStep / 2.0);
            qreal iconRadius = radius * 0.6;
            QPointF iconPos(iconRadius * qCos(iconAngle), iconRadius * qSin(iconAngle));
            
            QIcon icon = m_sectors[i].icon;
            if (!icon.isNull()) {
                QPixmap pixmap = icon.pixmap(32, 32);
                painter.drawPixmap(iconPos.toPoint() - QPoint(16, 16), pixmap);
            } else {
                // Placeholder dot
                painter.setBrush(Qt::white);
                painter.drawEllipse(iconPos, 4, 4);
            }
        }
    }
}

int KisFluentQuickMenu::calculateSector(QPointF pos) const
{
    QPointF center(width() / 2.0, height() / 2.0);
    QPointF delta = pos - center;
    
    if (qSqrt(delta.x() * delta.x() + delta.y() * delta.y()) < 20) {
        return -1; // Center deadzone
    }

    qreal angle = qRadiansToDegrees(qAtan2(delta.y(), delta.x()));
    angle += 90.0; // 0 degrees at top
    if (angle < 0) angle += 360.0;

    qreal angleStep = 360.0 / m_sectorCount;
    int sector = qRound((angle - angleStep / 2.0) / angleStep);
    if (sector < 0) sector += m_sectorCount;
    if (sector >= m_sectorCount) sector = 0;
    
    return sector;
}

void KisFluentQuickMenu::mousePressEvent(QMouseEvent *event)
{
    m_hoveredSector = calculateSector(event->pos());
    update();
}

void KisFluentQuickMenu::mouseMoveEvent(QMouseEvent *event)
{
    int newSector = calculateSector(event->pos());
    if (newSector != m_hoveredSector) {
        m_hoveredSector = newSector;
        update();
        // Conceptually: emit a signal to trigger device haptic feedback here
    }
}

void KisFluentQuickMenu::mouseReleaseEvent(QMouseEvent *event)
{
    int finalSector = calculateSector(event->pos());
    if (finalSector >= 0) {
        emit sectorActivated(finalSector);
        if (m_sectors.contains(finalSector) && m_sectors[finalSector].action) {
            m_sectors[finalSector].action->trigger();
        }
    }
    dismiss();
}
