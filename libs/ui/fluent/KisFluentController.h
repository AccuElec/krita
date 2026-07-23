/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 KritaFluent Contributors
 */

#ifndef KIS_FLUENT_CONTROLLER_H
#define KIS_FLUENT_CONTROLLER_H

#include <QObject>
#include <QPointer>
#include <QVariant>

class KisMainWindow;
class KisCanvas2;
class KisViewManager;
class KisFluentTopBar;
class KisFluentSliders;
class KisFluentCanvasOverlay;
class KisFluentInputEngine;

class KisFluentController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UiMode currentMode READ currentMode WRITE switchMode NOTIFY modeChanged)
    Q_PROPERTY(bool isFluentActive READ isFluentActive NOTIFY modeChanged)
    Q_PROPERTY(bool isTouchDevice READ isTouchDevice CONSTANT)
    Q_PROPERTY(InputType activeInputType READ activeInputType NOTIFY inputTypeChanged)
    Q_PROPERTY(Qt::LayoutDirection handedness READ handedness WRITE setHandedness NOTIFY handednessChanged)

public:
    enum UiMode { ClassicMode, FluentMode };
    Q_ENUM(UiMode)

    enum InputType { Mouse, Stylus, Touch };
    Q_ENUM(InputType)

    static KisFluentController* instance();

    void initialize(KisMainWindow *mainWindow);
    void switchMode(UiMode mode);
    void setHandedness(Qt::LayoutDirection dir);

    UiMode currentMode() const;
    bool isFluentActive() const;
    bool isTouchDevice() const;
    InputType activeInputType() const;
    Qt::LayoutDirection handedness() const;

    KisCanvas2* canvas() const;
    KisViewManager* viewManager() const;
    KisFluentTopBar* topBar() const;
    KisFluentSliders* sliders() const;
    KisFluentCanvasOverlay* overlay() const;
    KisFluentInputEngine* inputEngine() const;

signals:
    void modeChanged(UiMode newMode);
    void inputTypeChanged(InputType type);
    void handednessChanged(Qt::LayoutDirection dir);
    void fluentUiReady();

private:
    explicit KisFluentController(QObject *parent = nullptr);
    ~KisFluentController() override;
    
    Q_DISABLE_COPY(KisFluentController)

    void setupFluentWidgets();
    void teardownFluentWidgets();
    void detectInputCapabilities();
    void connectToMainWindow();

    UiMode m_currentMode { ClassicMode };
    bool m_isTouchDevice { false };
    InputType m_activeInputType { Mouse };
    Qt::LayoutDirection m_handedness { Qt::LeftToRight };

    QPointer<KisMainWindow> m_mainWindow;
    KisFluentTopBar *m_topBar { nullptr };
    KisFluentSliders *m_sliders { nullptr };
    KisFluentCanvasOverlay *m_overlay { nullptr };
    KisFluentInputEngine *m_inputEngine { nullptr };
};

#endif // KIS_FLUENT_CONTROLLER_H
