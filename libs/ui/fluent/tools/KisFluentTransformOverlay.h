/*
 * SPDX-FileCopyrightText: 2026 KritaFluent Contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KIS_FLUENT_TRANSFORM_OVERLAY_H
#define KIS_FLUENT_TRANSFORM_OVERLAY_H

#include <QWidget>
#include <QTransform>
#include <QRectF>
#include "kritaui_export.h"

/**
 * @brief Transparent overlay for transform operations in the KritaFluent UI.
 * 
 * Provides handles for scaling, skewing, moving, and rotating. Features
 * mode tabs (Free Transform, Perspective, etc.) and numeric input fields.
 * Supports both touch (pinch/twist) and desktop (drag, modifiers) input.
 */
class KRITAUI_EXPORT KisFluentTransformOverlay : public QWidget
{
    Q_OBJECT

public:
    enum TransformMode {
        ModeFreeTransform,
        ModePerspective,
        ModeWarp,
        ModeCage,
        ModeLiquify
    };
    Q_ENUM(TransformMode)

    explicit KisFluentTransformOverlay(QWidget *parent = nullptr);
    ~KisFluentTransformOverlay() override;

    TransformMode currentMode() const;
    QRectF transformBounds() const;
    qreal rotation() const;

public Q_SLOTS:
    void setMode(TransformMode mode);
    void setTransformBounds(const QRectF &bounds);
    void setRotation(qreal angle);
    void applyTransform();
    void cancelTransform();

Q_SIGNALS:
    void transformApplied(const QTransform &transform);
    void transformCancelled();
    void modeChanged(TransformMode mode);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool event(QEvent *event) override;

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // KIS_FLUENT_TRANSFORM_OVERLAY_H
