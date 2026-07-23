// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KIS_FLUENT_INPUT_ENGINE_H
#define KIS_FLUENT_INPUT_ENGINE_H

#include <QObject>
#include <QPointF>
#include <QPainterPath>
#include <QMap>
#include <QString>
#include <QScopedPointer>

class QWidget;
class QEvent;

/**
 * @brief The KisFluentInputEngine class
 * Handles touch, pen, and mouse inputs to provide adaptive gestures
 * and shape recognition for KritaFluent.
 */
class KisFluentInputEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(InputSource currentInputSource READ currentInputSource NOTIFY inputSourceChanged)
    Q_PROPERTY(bool penModeEnabled READ penModeEnabled WRITE setPenMode NOTIFY penModeChanged)
    Q_PROPERTY(bool quickShapeEnabled READ quickShapeEnabled WRITE setQuickShapeEnabled NOTIFY quickShapeEnabledChanged)

public:
    enum GestureType {
        TwoFingerTap,         // Undo
        ThreeFingerTap,       // Redo
        FourFingerTap,        // Toggle UI
        PinchZoom,
        TwoFingerRotate,
        TwoFingerPan,
        LongPress,            // Eyedropper
        TouchAndHold,         // QuickMenu
        ThreeFingerSwipeDown, // Cut/Copy/Paste
        QuickPinch,           // Fit to Screen
        DoubleTap,            // Fit to Screen
        StrokeHold            // QuickShape
    };
    Q_ENUM(GestureType)

    enum InputSource {
        PenInput,
        TouchInput,
        MouseInput
    };
    Q_ENUM(InputSource)

    struct GestureMapping {
        QString actionId;
    };

    explicit KisFluentInputEngine(QObject *parent = nullptr);
    ~KisFluentInputEngine() override;

    InputSource currentInputSource() const;
    bool penModeEnabled() const;
    bool quickShapeEnabled() const;

    /**
     * @brief Installs the event filter on the target canvas widget.
     * @param targetCanvas The canvas widget to listen to events on.
     */
    void install(QWidget *targetCanvas);

    /**
     * @brief Uninstalls the event filter from the currently installed canvas widget.
     */
    void uninstall();

    /**
     * @brief Maps a gesture type to a specific action id.
     * @param gesture The gesture to map.
     * @param actionId The action id to be executed when the gesture is recognized.
     */
    void setGestureMapping(GestureType gesture, const QString &actionId);

public slots:
    void setPenMode(bool enabled);
    void setQuickShapeEnabled(bool enabled);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void gestureRecognized(GestureType gesture, const QPointF &position);
    void inputSourceChanged(InputSource source);
    void quickShapeDetected(const QPainterPath &snappedShape);
    void eyedropperRequested(const QPointF &canvasPos);
    void quickMenuRequested(const QPointF &screenPos);
    
    void penModeChanged(bool enabled);
    void quickShapeEnabledChanged(bool enabled);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // KIS_FLUENT_INPUT_ENGINE_H
