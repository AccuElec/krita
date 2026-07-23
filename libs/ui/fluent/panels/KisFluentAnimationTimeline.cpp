// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "KisFluentAnimationTimeline.h"

#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QMenu>
#include <QWidgetAction>
#include <QSpinBox>
#include <QLabel>
#include <QGestureEvent>
#include <QPinchGesture>

KisFluentAnimationTimeline::KisFluentAnimationTimeline(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::TapAndHoldGesture);
}

KisFluentAnimationTimeline::~KisFluentAnimationTimeline() = default;

void KisFluentAnimationTimeline::setupUi()
{
    setFixedHeight(120);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(4);

    // Controls Row
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setContentsMargins(0, 0, 0, 0);

    m_playPauseButton = new QPushButton(tr("Play"), this);
    connect(m_playPauseButton, &QPushButton::clicked, this, &KisFluentAnimationTimeline::togglePlayback);

    m_loopButton = new QPushButton(tr("Loop"), this);
    m_loopButton->setCheckable(true);

    m_fpsSelector = new QComboBox(this);
    m_fpsSelector->addItems({"8", "12", "24", "30", "60"});
    m_fpsSelector->setCurrentText("24");
    connect(m_fpsSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        setFps(m_fpsSelector->currentText().toInt());
    });

    m_onionSkinButton = new QPushButton(tr("Onion"), this);
    m_onionSkinButton->setCheckable(true);
    connect(m_onionSkinButton, &QPushButton::toggled, this, &KisFluentAnimationTimeline::setOnionSkinEnabled);

    m_onionSkinOpacitySlider = new QSlider(Qt::Horizontal, this);
    m_onionSkinOpacitySlider->setRange(0, 100);
    m_onionSkinOpacitySlider->setValue(50);
    m_onionSkinOpacitySlider->setFixedWidth(100);

    m_addFrameButton = new QPushButton("+", this);
    connect(m_addFrameButton, &QPushButton::clicked, this, &KisFluentAnimationTimeline::addFrame);

    controlsLayout->addWidget(m_playPauseButton);
    controlsLayout->addWidget(m_loopButton);
    controlsLayout->addWidget(m_fpsSelector);
    controlsLayout->addWidget(m_onionSkinButton);
    controlsLayout->addWidget(m_onionSkinOpacitySlider);
    controlsLayout->addStretch();
    controlsLayout->addWidget(m_addFrameButton);

    mainLayout->addLayout(controlsLayout);

    // Timeline Row
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_framesContainer = new QWidget(m_scrollArea);
    m_framesContainer->setMinimumWidth(800); // Placeholder
    m_scrollArea->setWidget(m_framesContainer);
    
    m_framesContainer->installEventFilter(this);

    mainLayout->addWidget(m_scrollArea);

    m_slideAnimation = new QPropertyAnimation(this, "pos", this);
    m_slideAnimation->setDuration(250);
    m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

int KisFluentAnimationTimeline::currentFrame() const { return m_currentFrame; }
void KisFluentAnimationTimeline::setCurrentFrame(int frame)
{
    if (m_currentFrame == frame) return;
    m_currentFrame = frame;
    emit currentFrameChanged();
    emit frameSelected(m_currentFrame);
    m_framesContainer->update();
}

int KisFluentAnimationTimeline::frameCount() const { return m_frameCount; }
void KisFluentAnimationTimeline::setFrameCount(int count)
{
    if (m_frameCount == count) return;
    m_frameCount = count;
    emit frameCountChanged();
    m_framesContainer->setMinimumWidth(m_frameCount * 85 * m_zoomFactor);
    m_framesContainer->update();
}

int KisFluentAnimationTimeline::fps() const { return m_fps; }
void KisFluentAnimationTimeline::setFps(int newFps)
{
    if (m_fps == newFps) return;
    m_fps = newFps;
    emit fpsChanged(m_fps);
}

bool KisFluentAnimationTimeline::isPlaying() const { return m_isPlaying; }
void KisFluentAnimationTimeline::setIsPlaying(bool playing)
{
    if (m_isPlaying == playing) return;
    m_isPlaying = playing;
    m_playPauseButton->setText(m_isPlaying ? tr("Pause") : tr("Play"));
    emit isPlayingChanged();
    emit playbackToggled(m_isPlaying);
}

bool KisFluentAnimationTimeline::onionSkinEnabled() const { return m_onionSkinEnabled; }
void KisFluentAnimationTimeline::setOnionSkinEnabled(bool enabled)
{
    if (m_onionSkinEnabled == enabled) return;
    m_onionSkinEnabled = enabled;
    m_onionSkinButton->setChecked(m_onionSkinEnabled);
    emit onionSkinEnabledChanged();
}

void KisFluentAnimationTimeline::showPanel()
{
    if (m_isVisible) return;
    m_isVisible = true;
    show();
    if (parentWidget()) {
        m_slideAnimation->setStartValue(QPoint(pos().x(), parentWidget()->height()));
        m_slideAnimation->setEndValue(QPoint(pos().x(), parentWidget()->height() - height()));
        m_slideAnimation->start();
    }
    emit panelToggled(true);
}

void KisFluentAnimationTimeline::hidePanel()
{
    if (!m_isVisible) return;
    m_isVisible = false;
    if (parentWidget()) {
        m_slideAnimation->setStartValue(pos());
        m_slideAnimation->setEndValue(QPoint(pos().x(), parentWidget()->height()));
        m_slideAnimation->start();
    }
    // Simplification: In reality you might want to wait for animation finished before hiding.
    emit panelToggled(false);
}

void KisFluentAnimationTimeline::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(40, 40, 40));
}

bool KisFluentAnimationTimeline::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_framesContainer) {
        if (event->type() == QEvent::Paint) {
            QPainter painter(m_framesContainer);
            int frameWidth = 80 * m_zoomFactor;
            int frameHeight = 60;
            int spacing = 5 * m_zoomFactor;
            
            painter.setRenderHint(QPainter::Antialiasing);

            for (int i = 0; i < m_frameCount; ++i) {
                QRect frameRect(i * (frameWidth + spacing), 0, frameWidth, frameHeight);
                
                if (i == m_currentFrame) {
                    painter.fillRect(frameRect, QColor(60, 60, 60));
                    painter.setPen(QPen(QColor(0, 120, 215), 2));
                    painter.drawRect(frameRect.adjusted(1, 1, -1, -1));
                } else {
                    painter.fillRect(frameRect, QColor(50, 50, 50));
                    painter.setPen(QColor(100, 100, 100));
                    painter.drawRect(frameRect);
                }

                painter.setPen(QColor(200, 200, 200));
                painter.drawText(frameRect.adjusted(4, 4, -4, -4), Qt::AlignBottom | Qt::AlignHCenter, QString::number(i + 1));
            }
            return true;
        } else if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            int frameWidth = 80 * m_zoomFactor;
            int spacing = 5 * m_zoomFactor;
            int clickedFrame = mouseEvent->pos().x() / (frameWidth + spacing);
            if (clickedFrame >= 0 && clickedFrame < m_frameCount) {
                setCurrentFrame(clickedFrame);
            }
            return true;
        } else if (event->type() == QEvent::Gesture) {
            QGestureEvent *ge = static_cast<QGestureEvent*>(event);
            if (QGesture *pinch = ge->gesture(Qt::PinchGesture)) {
                QPinchGesture *pinchGesture = static_cast<QPinchGesture*>(pinch);
                m_zoomFactor *= pinchGesture->scaleFactor();
                m_zoomFactor = qBound(0.5f, m_zoomFactor, 3.0f);
                setFrameCount(m_frameCount); // trigger resize and update width
                m_framesContainer->update();
                return true;
            } else if (QGesture *tapAndHold = ge->gesture(Qt::TapAndHoldGesture)) {
                QTapAndHoldGesture *thGesture = static_cast<QTapAndHoldGesture*>(tapAndHold);
                int frameWidth = 80 * m_zoomFactor;
                int spacing = 5 * m_zoomFactor;
                QPoint pos = m_framesContainer->mapFromGlobal(thGesture->position().toPoint());
                int clickedFrame = pos.x() / (frameWidth + spacing);
                if (clickedFrame >= 0 && clickedFrame < m_frameCount) {
                    showFrameOptions(clickedFrame, thGesture->position().toPoint());
                }
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void KisFluentAnimationTimeline::togglePlayback()
{
    setIsPlaying(!m_isPlaying);
}

void KisFluentAnimationTimeline::toggleOnionSkin()
{
    setOnionSkinEnabled(!m_onionSkinEnabled);
}

void KisFluentAnimationTimeline::addFrame()
{
    setFrameCount(m_frameCount + 1);
    setCurrentFrame(m_currentFrame + 1);
    emit frameAdded(m_currentFrame);
}

void KisFluentAnimationTimeline::showFrameOptions(int frame, const QPoint &pos)
{
    QMenu menu(this);
    
    QWidgetAction *durationAction = new QWidgetAction(&menu);
    QWidget *durationWidget = new QWidget(&menu);
    QHBoxLayout *durationLayout = new QHBoxLayout(durationWidget);
    durationLayout->addWidget(new QLabel(tr("Hold Duration:")));
    QSpinBox *durationSpin = new QSpinBox();
    durationSpin->setRange(1, 100);
    durationLayout->addWidget(durationSpin);
    durationAction->setDefaultWidget(durationWidget);
    menu.addAction(durationAction);

    QAction *duplicateAct = menu.addAction(tr("Duplicate"));
    QAction *deleteAct = menu.addAction(tr("Delete"));
    menu.addSeparator();
    QAction *bgAct = menu.addAction(tr("Set as Background"));
    QAction *fgAct = menu.addAction(tr("Set as Foreground"));

    QAction *res = menu.exec(pos);
    if (res == duplicateAct) {
        setFrameCount(m_frameCount + 1);
        emit frameAdded(frame);
    } else if (res == deleteAct) {
        if (m_frameCount > 1) {
            setFrameCount(m_frameCount - 1);
            if (m_currentFrame >= m_frameCount) {
                setCurrentFrame(m_frameCount - 1);
            }
            emit frameDeleted(frame);
        }
    }
}
