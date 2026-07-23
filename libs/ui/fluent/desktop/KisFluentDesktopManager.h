// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KIS_FLUENT_DESKTOP_MANAGER_H
#define KIS_FLUENT_DESKTOP_MANAGER_H

#include <QObject>
#include <QWidget>

/**
 * @brief The KisFluentDesktopManager class
 * 
 * Manages Desktop-specific enhancements for KritaFluent.
 * Includes Multi-Monitor panel detachment helper, Keyboard Shortcut Visualizer,
 * and Graphics Tablet Pressure Curve visualizer widget.
 */
class KisFluentDesktopManager : public QObject
{
    Q_OBJECT

public:
    static KisFluentDesktopManager* instance();
    ~KisFluentDesktopManager() override;

    // Multi-Monitor panel detachment
    void detachPanelToScreen(QWidget *panel, int screenIndex);
    
    // Keyboard Shortcut Visualizer
    void toggleShortcutOverlay(bool visible);
    
    // Tablet Pressure
    QWidget* createPressureCurveWidget(QWidget *parent = nullptr);

signals:
    void detachedToScreen(QWidget *panel, int screenIndex);
    void shortcutOverlayToggled(bool visible);

private:
    explicit KisFluentDesktopManager(QObject *parent = nullptr);
    
    bool m_shortcutOverlayVisible;
};

#endif // KIS_FLUENT_DESKTOP_MANAGER_H
