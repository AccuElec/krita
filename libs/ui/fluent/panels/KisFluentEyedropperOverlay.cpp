// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "KisFluentEyedropperOverlay.h"

#include <QPainter>
#include <QPaintEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QPainterPath>

KisFluentEyedropperOverlay::KisFluentEyedropperOverlay(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
}

KisFluentEyedropperOverlay::~KisFluentEyedropperOverlay() = default;

void KisFluentEyedropperOverlay::activate(QPointF pos)
{
    m_active = true;
    m_pos = pos;
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        m_screenCache = screen->grabWindow(0);
    }
    showFullScreen();
    update();
}

void KisFluentEyedropperOverlay::updatePosition(QPointF pos)
{
    if (m_active) {
        m_pos = pos;
        update();
    }
}

void KisFluentEyedropperOverlay::deactivate()
{
    m_active = false;
    hide();
    Q_EMIT colorPicked(m_newColor);
}

void KisFluentEyedropperOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (!m_active) return;
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Magnifying loupe logic
    const qreal radius = 50.0;
    
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(m_pos, radius, radius);
    
    // Draw sampled content and color preview logic here...
}
