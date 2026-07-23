// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#ifndef KIS_FLUENT_CANVAS_PRESET_PICKER_H
#define KIS_FLUENT_CANVAS_PRESET_PICKER_H

#include <QWidget>
#include <QString>
#include <QColor>

/**
 * @brief Modal overlay for creating new canvases with presets or custom sizes.
 */
class KisFluentCanvasPresetPicker : public QWidget
{
    Q_OBJECT
public:
    explicit KisFluentCanvasPresetPicker(QWidget *parent = nullptr);
    ~KisFluentCanvasPresetPicker() override;

signals:
    /**
     * @brief Emitted when a new canvas should be created.
     */
    void canvasCreated(int width, int height, int dpi, const QString &colorProfile, const QColor &bgColor);

    /**
     * @brief Emitted when canvas creation is cancelled.
     */
    void cancelled();

protected:
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif // KIS_FLUENT_CANVAS_PRESET_PICKER_H
