/*
 * SPDX-FileCopyrightText: 2026 KritaFluent Contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KisFluentTransformOverlay.h"

#include <QPainter>
#include <QMouseEvent>
#include <QTouchEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

class KisFluentTransformOverlay::Private
{
public:
    TransformMode mode {ModeFreeTransform};
    QRectF bounds;
    qreal rotation {0.0};
    
    QPushButton *btnConfirm {nullptr};
    QPushButton *btnCancel {nullptr};

    bool isDragging {false};
    QPointF lastPos;
};

KisFluentTransformOverlay::KisFluentTransformOverlay(QWidget *parent)
    : QWidget(parent)
    , d(new Private())
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_AcceptTouchEvents, true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    
    d->btnCancel = new QPushButton("Cancel", this);
    d->btnConfirm = new QPushButton("Confirm", this);
    
    connect(d->btnCancel, &QPushButton::clicked, this, &KisFluentTransformOverlay::cancelTransform);
    connect(d->btnConfirm, &QPushButton::clicked, this, &KisFluentTransformOverlay::applyTransform);
    
    bottomLayout->addWidget(d->btnCancel);
    bottomLayout->addWidget(d->btnConfirm);
    
    mainLayout->addLayout(bottomLayout);
}

KisFluentTransformOverlay::~KisFluentTransformOverlay()
{
}

KisFluentTransformOverlay::TransformMode KisFluentTransformOverlay::currentMode() const
{
    return d->mode;
}

QRectF KisFluentTransformOverlay::transformBounds() const
{
    return d->bounds;
}

qreal KisFluentTransformOverlay::rotation() const
{
    return d->rotation;
}

void KisFluentTransformOverlay::setMode(TransformMode mode)
{
    if (d->mode != mode) {
        d->mode = mode;
        Q_EMIT modeChanged(mode);
        update();
    }
}

void KisFluentTransformOverlay::setTransformBounds(const QRectF &bounds)
{
    if (d->bounds != bounds) {
        d->bounds = bounds;
        update();
    }
}

void KisFluentTransformOverlay::setRotation(qreal angle)
{
    if (!qFuzzyCompare(d->rotation, angle)) {
        d->rotation = angle;
        update();
    }
}

void KisFluentTransformOverlay::applyTransform()
{
    QTransform t;
    t.translate(d->bounds.center().x(), d->bounds.center().y());
    t.rotate(d->rotation);
    t.translate(-d->bounds.center().x(), -d->bounds.center().y());
    
    Q_EMIT transformApplied(t);
}

void KisFluentTransformOverlay::cancelTransform()
{
    Q_EMIT transformCancelled();
}

void KisFluentTransformOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (d->bounds.isEmpty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.save();
    painter.translate(d->bounds.center());
    painter.rotate(d->rotation);
    painter.translate(-d->bounds.center());

    painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
    painter.drawRect(d->bounds);

    painter.setBrush(Qt::white);
    painter.setPen(Qt::black);

    const int handleSize = 8;
    const int hs2 = handleSize / 2;

    painter.drawRect(d->bounds.topLeft().x() - hs2, d->bounds.topLeft().y() - hs2, handleSize, handleSize);
    painter.drawRect(d->bounds.topRight().x() - hs2, d->bounds.topRight().y() - hs2, handleSize, handleSize);
    painter.drawRect(d->bounds.bottomLeft().x() - hs2, d->bounds.bottomLeft().y() - hs2, handleSize, handleSize);
    painter.drawRect(d->bounds.bottomRight().x() - hs2, d->bounds.bottomRight().y() - hs2, handleSize, handleSize);

    QPointF topCenter(d->bounds.center().x(), d->bounds.top());
    painter.drawLine(topCenter, QPointF(topCenter.x(), topCenter.y() - 30));
    painter.drawEllipse(QPointF(topCenter.x(), topCenter.y() - 30), hs2, hs2);

    painter.restore();
}

void KisFluentTransformOverlay::mousePressEvent(QMouseEvent *event)
{
    d->isDragging = true;
    d->lastPos = event->pos();
}

void KisFluentTransformOverlay::mouseMoveEvent(QMouseEvent *event)
{
    if (d->isDragging) {
        QPointF delta = event->pos() - d->lastPos;
        d->bounds.translate(delta.x(), delta.y());
        d->lastPos = event->pos();
        update();
    }
}

void KisFluentTransformOverlay::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    d->isDragging = false;
}

bool KisFluentTransformOverlay::event(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin || 
        event->type() == QEvent::TouchUpdate || 
        event->type() == QEvent::TouchEnd) {
        event->accept();
        return true;
    }
    return QWidget::event(event);
}
