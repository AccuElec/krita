/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 KritaFluent Contributors
 */

#ifndef KIS_FLUENT_CANVAS_OVERLAY_H
#define KIS_FLUENT_CANVAS_OVERLAY_H

#include <QWidget>
#include <QPainterPath>
#include <QColor>
#include <QPointF>
#include <QPropertyAnimation>

/**
 * @brief Transparent overlay positioned on top of the canvas for fluent visual feedback.
 * 
 * Handles brush cursor rendering, gesture feedback flashes, eyedropper loupe,
 * and quick shape morphing animations.
 */
class KisFluentCanvasOverlay : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal flashOpacity READ flashOpacity WRITE setFlashOpacity)
    Q_PROPERTY(qreal morphProgress READ morphProgress WRITE setMorphProgress)

public:
    explicit KisFluentCanvasOverlay(QWidget *parent = nullptr);
    ~KisFluentCanvasOverlay() override;

    /**
     * @brief Updates the brush cursor display parameters.
     * @param pos Cursor position in widget coordinates.
     * @param size Size of the brush cursor in pixels.
     * @param opacity Opacity of the brush (0.0 to 1.0).
     */
    void updateCursor(QPointF pos, float size, float opacity);

    /**
     * @brief Shows a subtle white flash to indicate an Undo action.
     */
    void showUndoFlash();

    /**
     * @brief Shows a subtle blue flash to indicate a Redo action.
     */
    void showRedoFlash();

    /**
     * @brief Displays the eyedropper magnifier loupe.
     * @param pos Position of the eyedropper in widget coordinates.
     * @param sampledColor The color currently being sampled.
     * @param currentColor The original color before sampling began.
     */
    void showEyedropper(QPointF pos, QColor sampledColor, QColor currentColor);

    /**
     * @brief Hides the eyedropper loupe.
     */
    void hideEyedropper();

    /**
     * @brief Shows the morphing animation for QuickShape snapping.
     * @param original The original drawn path.
     * @param snapped The geometric snapped path.
     */
    void showQuickShapePreview(QPainterPath original, QPainterPath snapped);

    qreal flashOpacity() const;
    void setFlashOpacity(qreal opacity);

    qreal morphProgress() const;
    void setMorphProgress(qreal progress);

    void setShowBrushCursor(bool show);
    bool showBrushCursor() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float m_brushSize;
    float m_brushOpacity;
    QPointF m_cursorPosition;
    bool m_showBrushCursor;

    QColor m_flashColor;
    qreal m_flashOpacity;
    QPropertyAnimation *m_flashAnimation;

    bool m_showEyedropper;
    QPointF m_eyedropperPos;
    QColor m_eyedropperSampledColor;
    QColor m_eyedropperCurrentColor;

    bool m_showMorph;
    QPainterPath m_morphOriginal;
    QPainterPath m_morphSnapped;
    qreal m_morphProgress;
    QPropertyAnimation *m_morphAnimation;
};

#endif // KIS_FLUENT_CANVAS_OVERLAY_H
