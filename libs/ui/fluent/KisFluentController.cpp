/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 KritaFluent Contributors
 */

#include "KisFluentController.h"

#include <QGuiApplication>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QInputDevice>
#else
#include <QTouchDevice>
#endif

// Forward declare for the implementation details we might use.
// Usually we'd include these real headers:
// #include "KisMainWindow.h"
// #include "KisCanvas2.h"
// #include "KisViewManager.h"
// #include "KisFluentTopBar.h"
// #include "KisFluentSliders.h"
// #include "KisFluentCanvasOverlay.h"
// #include "KisFluentInputEngine.h"

KisFluentController* KisFluentController::instance()
{
    static KisFluentController s_instance;
    return &s_instance;
}

KisFluentController::KisFluentController(QObject *parent)
    : QObject(parent)
{
}

KisFluentController::~KisFluentController()
{
    teardownFluentWidgets();
}

void KisFluentController::initialize(KisMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
    
    detectInputCapabilities();
    connectToMainWindow();

    // Suggest Fluent mode if a touch device is detected
    if (m_isTouchDevice) {
        // switchMode(FluentMode); // Could be invoked depending on preferences
    }
}

void KisFluentController::switchMode(UiMode mode)
{
    if (m_currentMode == mode) return;

    m_currentMode = mode;
    
    if (mode == FluentMode) {
        // Hide classic dockers/toolbars here
        setupFluentWidgets();
    } else {
        teardownFluentWidgets();
        // Restore classic UI here
    }
    
    emit modeChanged(m_currentMode);
}

void KisFluentController::setHandedness(Qt::LayoutDirection dir)
{
    if (m_handedness == dir) return;
    m_handedness = dir;
    
    // if (m_topBar) m_topBar->setLayoutDirection(dir);
    // if (m_sliders) m_sliders->setLayoutDirection(dir);
    // if (m_overlay) m_overlay->setLayoutDirection(dir);
    
    emit handednessChanged(dir);
}

KisFluentController::UiMode KisFluentController::currentMode() const
{
    return m_currentMode;
}

bool KisFluentController::isFluentActive() const
{
    return m_currentMode == FluentMode;
}

bool KisFluentController::isTouchDevice() const
{
    return m_isTouchDevice;
}

KisFluentController::InputType KisFluentController::activeInputType() const
{
    return m_activeInputType;
}

Qt::LayoutDirection KisFluentController::handedness() const
{
    return m_handedness;
}

KisCanvas2* KisFluentController::canvas() const
{
    // return m_mainWindow ? m_mainWindow->activeCanvas() : nullptr;
    return nullptr;
}

KisViewManager* KisFluentController::viewManager() const
{
    // return m_mainWindow ? m_mainWindow->viewManager() : nullptr;
    return nullptr;
}

KisFluentTopBar* KisFluentController::topBar() const
{
    return m_topBar;
}

KisFluentSliders* KisFluentController::sliders() const
{
    return m_sliders;
}

KisFluentCanvasOverlay* KisFluentController::overlay() const
{
    return m_overlay;
}

KisFluentInputEngine* KisFluentController::inputEngine() const
{
    return m_inputEngine;
}

void KisFluentController::setupFluentWidgets()
{
    if (!m_mainWindow) return;

    // m_topBar = new KisFluentTopBar(m_mainWindow);
    // m_sliders = new KisFluentSliders(m_mainWindow);
    // m_overlay = new KisFluentCanvasOverlay(m_mainWindow);
    // m_inputEngine = new KisFluentInputEngine(this);

    // Positions them relative to the canvas widget...

    emit fluentUiReady();
}

void KisFluentController::teardownFluentWidgets()
{
    // if (m_topBar) { m_topBar->deleteLater(); m_topBar = nullptr; }
    // if (m_sliders) { m_sliders->deleteLater(); m_sliders = nullptr; }
    // if (m_overlay) { m_overlay->deleteLater(); m_overlay = nullptr; }
    // if (m_inputEngine) { m_inputEngine->deleteLater(); m_inputEngine = nullptr; }
}

void KisFluentController::detectInputCapabilities()
{
    m_isTouchDevice = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    for (const QInputDevice *dev : QInputDevice::devices()) {
        if (dev->type() == QInputDevice::DeviceType::TouchScreen) {
            m_isTouchDevice = true;
            break;
        }
    }
#else
    for (const QTouchDevice *dev : QTouchDevice::devices()) {
        if (dev->type() == QTouchDevice::TouchScreen) {
            m_isTouchDevice = true;
            break;
        }
    }
#endif
}

void KisFluentController::connectToMainWindow()
{
    // Hook into existing Krita signals from m_mainWindow
}
