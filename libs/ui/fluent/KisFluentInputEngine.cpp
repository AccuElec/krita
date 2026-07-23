// SPDX-License-Identifier: GPL-3.0-or-later

#include "KisFluentInputEngine.h"

#include <QWidget>
#include <QEvent>
#include <QTouchEvent>
#include <QTabletEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QMap>

class KisFluentInputEngine::Private {
public:
    QWidget *targetCanvas = nullptr;
    KisFluentInputEngine::InputSource currentInputSource = KisFluentInputEngine::MouseInput;
    bool penModeEnabled = true;
    bool quickShapeEnabled = true;
    QMap<KisFluentInputEngine::GestureType, QString> gestureMappings;
    
    // Touch state
    QElapsedTimer touchTimer;
    int maxTouchPoints = 0;
    QPointF initialTouchPos;
    bool touchMoved = false;
    QTimer longPressTimer;
    
    // Stroke/QuickShape state
    QTimer strokeHoldTimer;
    QPainterPath currentStrokePath;
};

KisFluentInputEngine::KisFluentInputEngine(QObject *parent)
    : QObject(parent)
    , d(new Private())
{
    // Setup Long Press Timer (Eyedropper)
    d->longPressTimer.setSingleShot(true);
    d->longPressTimer.setInterval(500); // 500ms for eyedropper
    connect(&d->longPressTimer, &QTimer::timeout, this, [this]() {
        if (!d->touchMoved && d->maxTouchPoints == 1) {
            emit eyedropperRequested(d->initialTouchPos);
        }
    });

    // Setup Stroke Hold Timer (QuickShape)
    d->strokeHoldTimer.setSingleShot(true);
    d->strokeHoldTimer.setInterval(500); // 500ms after pen lift staying at endpoint
    connect(&d->strokeHoldTimer, &QTimer::timeout, this, [this]() {
        if (d->quickShapeEnabled && !d->currentStrokePath.isEmpty()) {
            // Simplified shape analysis: finding the bounding rect
            // A more complex implementation would calculate curvature variance, detect corners, etc.
            QPainterPath snappedShape;
            snappedShape.addRect(d->currentStrokePath.boundingRect());
            emit quickShapeDetected(snappedShape);
        }
    });
}

KisFluentInputEngine::~KisFluentInputEngine() = default;

KisFluentInputEngine::InputSource KisFluentInputEngine::currentInputSource() const
{
    return d->currentInputSource;
}

bool KisFluentInputEngine::penModeEnabled() const
{
    return d->penModeEnabled;
}

void KisFluentInputEngine::setPenMode(bool enabled)
{
    if (d->penModeEnabled != enabled) {
        d->penModeEnabled = enabled;
        emit penModeChanged(enabled);
    }
}

bool KisFluentInputEngine::quickShapeEnabled() const
{
    return d->quickShapeEnabled;
}

void KisFluentInputEngine::setQuickShapeEnabled(bool enabled)
{
    if (d->quickShapeEnabled != enabled) {
        d->quickShapeEnabled = enabled;
        emit quickShapeEnabledChanged(enabled);
    }
}

void KisFluentInputEngine::install(QWidget *targetCanvas)
{
    if (d->targetCanvas) {
        uninstall();
    }
    d->targetCanvas = targetCanvas;
    if (d->targetCanvas) {
        d->targetCanvas->installEventFilter(this);
        d->targetCanvas->setAttribute(Qt::WA_AcceptTouchEvents, true);
    }
}

void KisFluentInputEngine::uninstall()
{
    if (d->targetCanvas) {
        d->targetCanvas->removeEventFilter(this);
        d->targetCanvas = nullptr;
    }
}

void KisFluentInputEngine::setGestureMapping(GestureType gesture, const QString &actionId)
{
    d->gestureMappings[gesture] = actionId;
}

bool KisFluentInputEngine::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != d->targetCanvas) {
        return QObject::eventFilter(watched, event);
    }

    switch (event->type()) {
        case QEvent::TabletPress:
        case QEvent::TabletMove:
        case QEvent::TabletRelease: {
            auto *tabletEvent = static_cast<QTabletEvent*>(event);
            
            if (d->currentInputSource != PenInput) {
                d->currentInputSource = PenInput;
                emit inputSourceChanged(PenInput);
            }
            
            // Stroke tracking for QuickShape
            if (event->type() == QEvent::TabletPress) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                d->currentStrokePath = QPainterPath(tabletEvent->position());
#else
                d->currentStrokePath = QPainterPath(tabletEvent->posF());
#endif
                d->strokeHoldTimer.stop();
            } else if (event->type() == QEvent::TabletMove) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                d->currentStrokePath.lineTo(tabletEvent->position());
#else
                d->currentStrokePath.lineTo(tabletEvent->posF());
#endif
                d->strokeHoldTimer.stop();
            } else if (event->type() == QEvent::TabletRelease) {
                if (d->quickShapeEnabled) {
                    d->strokeHoldTimer.start();
                }
            }
            
            // Do not consume, let Krita handle the painting
            return false;
        }
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
        case QEvent::TouchCancel: {
            auto *touchEvent = static_cast<QTouchEvent*>(event);
            
            if (d->currentInputSource != TouchInput) {
                d->currentInputSource = TouchInput;
                emit inputSourceChanged(TouchInput);
            }
            
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            const auto &touchPoints = touchEvent->points();
#else
            const auto &touchPoints = touchEvent->touchPoints();
#endif
            int currentPoints = 0;
            for (const auto &tp : touchPoints) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                if (tp.state() != QEventPoint::State::Released) {
                    currentPoints++;
                }
#else
                if (tp.state() != Qt::TouchPointReleased) {
                    currentPoints++;
                }
#endif
            }
            
            if (event->type() == QEvent::TouchBegin) {
                d->touchTimer.start();
                d->maxTouchPoints = currentPoints;
                d->touchMoved = false;
                if (currentPoints == 1) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                    d->initialTouchPos = touchPoints.first().position();
#else
                    d->initialTouchPos = touchPoints.first().pos();
#endif
                    d->longPressTimer.start();
                }
            } else if (event->type() == QEvent::TouchUpdate) {
                if (currentPoints > d->maxTouchPoints) {
                    d->maxTouchPoints = currentPoints;
                }
                
                if (currentPoints == 1) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                    QPointF currentPos = touchPoints.first().position();
                    QPointF screenPos = touchPoints.first().globalPosition();
#else
                    QPointF currentPos = touchPoints.first().pos();
                    QPointF screenPos = touchPoints.first().screenPos();
#endif
                    
                    if ((currentPos - d->initialTouchPos).manhattanLength() > 20) {
                        d->touchMoved = true;
                        if (d->longPressTimer.isActive()) {
                            d->longPressTimer.stop();
                        }
                        
                        // Touch-and-hold QuickMenu > 400ms then move
                        if (d->touchTimer.elapsed() > 400) {
                            emit quickMenuRequested(screenPos);
                            return true; // Consume event to prevent canvas pan
                        }
                    }
                } else if (currentPoints >= 2) {
                    d->longPressTimer.stop();
                    // In a full implementation, detect pinch and rotation here
                    // emit gestureRecognized(PinchZoom, ...);
                }
            } else if (event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchCancel) {
                d->longPressTimer.stop();
                
                if (currentPoints == 0) {
                    qint64 elapsed = d->touchTimer.elapsed();
                    // Tap detection (<=300ms, <20px movement)
                    if (elapsed <= 300 && !d->touchMoved) {
                        if (d->maxTouchPoints == 2) {
                            emit gestureRecognized(TwoFingerTap, d->initialTouchPos);
                            return true;
                        } else if (d->maxTouchPoints == 3) {
                            emit gestureRecognized(ThreeFingerTap, d->initialTouchPos);
                            return true;
                        } else if (d->maxTouchPoints == 4) {
                            emit gestureRecognized(FourFingerTap, d->initialTouchPos);
                            return true;
                        }
                    }
                }
            }
            
            // If pen mode is enabled, consume touch events to prevent unintentional painting
            if (d->penModeEnabled) {
                // In a robust implementation, you might still let some navigation 
                // events through or handle them separately.
            }
            
            return false;
        }
        case QEvent::MouseButtonPress:
        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease: {
            auto *mouseEvent = static_cast<QMouseEvent*>(event);
            
            // Ignore synthesized mouse events from touch/tablet
            if (mouseEvent->source() == Qt::MouseEventNotSynthesized) {
                if (d->currentInputSource != MouseInput) {
                    d->currentInputSource = MouseInput;
                    emit inputSourceChanged(MouseInput);
                }
                
                if (event->type() == QEvent::MouseButtonPress) {
                    if (mouseEvent->button() == Qt::MiddleButton) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                        emit quickMenuRequested(mouseEvent->globalPosition());
#else
                        emit quickMenuRequested(mouseEvent->globalPos());
#endif
                        return true;
                    }
                }
            }
            return false;
        }
        case QEvent::Wheel: {
            if (d->currentInputSource != MouseInput) {
                d->currentInputSource = MouseInput;
                emit inputSourceChanged(MouseInput);
            }
            // Let Krita handle standard wheel events for zooming/brush size
            return false;
        }
        default:
            break;
    }

    return QObject::eventFilter(watched, event);
}
