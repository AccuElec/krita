// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KIS_FLUENT_EYEDROPPER_OVERLAY_H
#define KIS_FLUENT_EYEDROPPER_OVERLAY_H

#include <QWidget>
#include <QColor>
#include <QPointF>
#include <QPixmap>

/**
 * @class KisFluentEyedropperOverlay
 * @brief Transparent overlay widget for eyedropper mode.
 */
class KisFluentEyedropperOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit KisFluentEyedropperOverlay(QWidget *parent = nullptr);
    ~KisFluentEyedropperOverlay() override;

public Q_SLOTS:
    void activate(QPointF pos);
    void updatePosition(QPointF pos);
    void deactivate();

Q_SIGNALS:
    void colorPicked(QColor color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPointF m_pos;
    bool m_active{false};
    QColor m_currentColor;
    QColor m_newColor;
    QPixmap m_screenCache;
};

#endif // KIS_FLUENT_EYEDROPPER_OVERLAY_H
