// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#ifndef KIS_FLUENT_BRUSH_STUDIO_H
#define KIS_FLUENT_BRUSH_STUDIO_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QTabWidget>
#include <QVBoxLayout>

/**
 * @brief Full-screen overlay for deep brush customization
 * 
 * Provides a tabbed interface for editing brush properties with
 * large touch-friendly sliders and a live preview canvas.
 */
class KisFluentBrushStudio : public QWidget
{
    Q_OBJECT

public:
    explicit KisFluentBrushStudio(QWidget *parent = nullptr);
    ~KisFluentBrushStudio() override;

    void loadPreset(const QString& presetName);
    void saveAsNewPreset(const QString& name);

public slots:
    void show();
    void hide();

signals:
    void presetModified();
    void presetSaved(const QString& name);
    void closed();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUi();
    void setupTabs();

    QPropertyAnimation *m_slideAnimation;
    QTabWidget *m_tabWidget;
    QWidget *m_previewCanvas;
};

#endif // KIS_FLUENT_BRUSH_STUDIO_H
