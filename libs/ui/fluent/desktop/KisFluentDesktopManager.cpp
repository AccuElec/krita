// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "KisFluentDesktopManager.h"

#include <QGuiApplication>
#include <QScreen>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

KisFluentDesktopManager* KisFluentDesktopManager::instance()
{
    static KisFluentDesktopManager s_instance;
    return &s_instance;
}

KisFluentDesktopManager::KisFluentDesktopManager(QObject *parent)
    : QObject(parent)
    , m_shortcutOverlayVisible(false)
{
}

KisFluentDesktopManager::~KisFluentDesktopManager()
{
}

void KisFluentDesktopManager::detachPanelToScreen(QWidget *panel, int screenIndex)
{
    if (!panel) return;
    
    QList<QScreen *> screens = QGuiApplication::screens();
    if (screenIndex >= 0 && screenIndex < screens.size()) {
        QScreen *targetScreen = screens.at(screenIndex);
        
        panel->setParent(nullptr);
        panel->setWindowFlags(Qt::Window | Qt::Tool);
        
        // Move to the center of the target screen
        QRect screenGeom = targetScreen->geometry();
        int x = screenGeom.x() + (screenGeom.width() - panel->width()) / 2;
        int y = screenGeom.y() + (screenGeom.height() - panel->height()) / 2;
        
        panel->move(x, y);
        panel->show();
        
        emit detachedToScreen(panel, screenIndex);
    }
}

void KisFluentDesktopManager::toggleShortcutOverlay(bool visible)
{
    if (m_shortcutOverlayVisible != visible) {
        m_shortcutOverlayVisible = visible;
        // In a full implementation, this would show/hide the on-screen hotkey overlay
        emit shortcutOverlayToggled(m_shortcutOverlayVisible);
    }
}

QWidget* KisFluentDesktopManager::createPressureCurveWidget(QWidget *parent)
{
    // Placeholder for Graphics Tablet Pressure Curve visualizer widget
    QWidget *widget = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    QLabel *label = new QLabel("Pressure Curve Visualizer", widget);
    label->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(label);
    
    widget->setStyleSheet("background-color: #333; color: white; border-radius: 8px;");
    widget->setMinimumSize(200, 150);
    
    return widget;
}
