// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "KisFluentGestureGuide.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QGestureEvent>

KisFluentGestureGuide::KisFluentGestureGuide(QWidget *parent)
    : QWidget(parent)
{
    // Full screen overlay
    if (parent) {
        resize(parent->size());
    }
    
    setupUi();
    updateStepContent();
}

KisFluentGestureGuide::~KisFluentGestureGuide() = default;

void KisFluentGestureGuide::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Top Row
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch();
    m_skipButton = new QPushButton(tr("Skip Tutorial"), this);
    m_skipButton->setFlat(true);
    m_skipButton->setStyleSheet("color: white; font-weight: bold;");
    connect(m_skipButton, &QPushButton::clicked, this, &KisFluentGestureGuide::skipTutorial);
    topLayout->addWidget(m_skipButton);
    mainLayout->addLayout(topLayout);

    // Card Container
    QWidget *card = new QWidget(this);
    card->setStyleSheet("background-color: #2D2D30; border-radius: 12px;");
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 32, 32, 32);
    cardLayout->setAlignment(Qt::AlignHCenter);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_subtitleLabel = new QLabel(this);
    m_subtitleLabel->setStyleSheet("color: #CCCCCC; font-size: 16px;");
    m_subtitleLabel->setAlignment(Qt::AlignCenter);
    m_subtitleLabel->setWordWrap(true);

    m_animationArea = new QWidget(this);
    m_animationArea->setFixedSize(200, 200);
    m_animationArea->installEventFilter(this);

    m_practiceArea = new QWidget(this);
    m_practiceArea->setFixedSize(400, 200);
    m_practiceArea->setStyleSheet("background-color: #1E1E1E; border: 2px dashed #555555; border-radius: 8px;");
    m_practiceArea->installEventFilter(this);
    // Grab some gestures for the practice area
    m_practiceArea->grabGesture(Qt::PinchGesture);
    m_practiceArea->grabGesture(Qt::TapAndHoldGesture);

    m_feedbackLabel = new QLabel(this);
    m_feedbackLabel->setAlignment(Qt::AlignCenter);
    m_feedbackLabel->setStyleSheet("color: #4CAF50; font-size: 18px; font-weight: bold;");
    m_feedbackLabel->hide();

    cardLayout->addWidget(m_titleLabel);
    cardLayout->addWidget(m_subtitleLabel);
    cardLayout->addWidget(m_animationArea, 0, Qt::AlignHCenter);
    cardLayout->addWidget(new QLabel(tr("Try it below:"), this), 0, Qt::AlignHCenter);
    cardLayout->addWidget(m_practiceArea, 0, Qt::AlignHCenter);
    cardLayout->addWidget(m_feedbackLabel);

    mainLayout->addWidget(card, 1, Qt::AlignCenter);

    // Bottom Row
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    
    m_dontShowAgainCheck = new QCheckBox(tr("Don't show again"), this);
    m_dontShowAgainCheck->setStyleSheet("color: white;");
    m_dontShowAgainCheck->hide();
    
    m_dotsContainer = new QWidget(this);
    QHBoxLayout *dotsLayout = new QHBoxLayout(m_dotsContainer);
    for (int i = 0; i < m_totalSteps; ++i) {
        QLabel *dot = new QLabel("•", m_dotsContainer);
        dot->setStyleSheet("color: #555555; font-size: 24px;");
        dotsLayout->addWidget(dot);
    }

    m_nextButton = new QPushButton(tr("Next"), this);
    m_nextButton->setStyleSheet("background-color: #0078D7; color: white; border-radius: 4px; padding: 8px 24px;");
    connect(m_nextButton, &QPushButton::clicked, this, &KisFluentGestureGuide::nextStep);

    bottomLayout->addWidget(m_dontShowAgainCheck);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_dotsContainer);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_nextButton);

    mainLayout->addLayout(bottomLayout);

    // Dummy animation for illustration
    m_illustrationAnimation = new QPropertyAnimation(this, "pos", this);
    m_illustrationAnimation->setDuration(2000);
    m_illustrationAnimation->setLoopCount(-1);
    connect(m_illustrationAnimation, &QPropertyAnimation::valueChanged, this, &KisFluentGestureGuide::onAnimationTick);
    m_illustrationAnimation->start();
}

int KisFluentGestureGuide::currentStep() const { return m_currentStep; }
int KisFluentGestureGuide::totalSteps() const { return m_totalSteps; }

void KisFluentGestureGuide::setCurrentStep(int step)
{
    if (m_currentStep == step || step < 0 || step >= m_totalSteps) return;
    m_currentStep = step;
    m_stepSuccessful = false;
    m_feedbackLabel->hide();
    updateStepContent();
    emit currentStepChanged();
}

void KisFluentGestureGuide::updateStepContent()
{
    QString title, subtitle;
    switch (m_currentStep) {
        case 0:
            title = tr("Pinch to Zoom");
            subtitle = tr("Use two fingers to pinch in or out to zoom the canvas.");
            break;
        case 1:
            title = tr("Two-finger Twist");
            subtitle = tr("Rotate two fingers to rotate the canvas.");
            break;
        case 2:
            title = tr("Two-finger Tap");
            subtitle = tr("Tap with two fingers to undo the last action.");
            break;
        case 3:
            title = tr("Three-finger Tap");
            subtitle = tr("Tap with three fingers to redo.");
            break;
        case 4:
            title = tr("Long Press");
            subtitle = tr("Press and hold to pick a color (Eyedropper).");
            break;
        case 5:
            title = tr("Touch-and-Hold");
            subtitle = tr("Touch and hold with one finger, tap with another for QuickMenu.");
            break;
        case 6:
            title = tr("Hold at Stroke End");
            subtitle = tr("Draw a shape and hold at the end to snap to a QuickShape.");
            break;
    }

    m_titleLabel->setText(title);
    m_subtitleLabel->setText(subtitle);

    // Update dots
    QLayout *dotsLayout = m_dotsContainer->layout();
    for (int i = 0; i < dotsLayout->count(); ++i) {
        QLabel *dot = qobject_cast<QLabel*>(dotsLayout->itemAt(i)->widget());
        if (dot) {
            dot->setStyleSheet(i == m_currentStep ? "color: white; font-size: 24px;" : "color: #555555; font-size: 24px;");
        }
    }

    if (m_currentStep == m_totalSteps - 1) {
        m_dontShowAgainCheck->show();
        m_nextButton->setText(tr("Finish"));
    } else {
        m_dontShowAgainCheck->hide();
        m_nextButton->setText(tr("Next"));
    }
}

void KisFluentGestureGuide::nextStep()
{
    if (m_currentStep < m_totalSteps - 1) {
        emit stepCompleted(m_currentStep);
        setCurrentStep(m_currentStep + 1);
    } else {
        emit tutorialCompleted();
        hide();
        deleteLater();
    }
}

void KisFluentGestureGuide::skipTutorial()
{
    emit tutorialSkipped();
    hide();
    deleteLater();
}

void KisFluentGestureGuide::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    // Dark overlay
    painter.fillRect(rect(), QColor(0, 0, 0, 180));
    QWidget::paintEvent(event);
}

bool KisFluentGestureGuide::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_animationArea && event->type() == QEvent::Paint) {
        QPainter painter(m_animationArea);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::white, 2));
        
        // Simple procedural animation drawing based on current step
        int offset = (m_animTick % 100) - 50;
        
        if (m_currentStep == 0) { // Pinch
            painter.drawEllipse(100 - 20 - offset/2, 100, 20, 20);
            painter.drawEllipse(100 + 20 + offset/2, 100, 20, 20);
        } else if (m_currentStep == 1) { // Rotate
            painter.drawArc(50, 50, 100, 100, offset * 16, 180 * 16);
        } else {
            // Placeholder for other animations
            painter.drawEllipse(100, 100, 30, 30);
        }
        return true;
    }

    if (watched == m_practiceArea) {
        if (event->type() == QEvent::Gesture) {
            // Simplified gesture detection for the practice area
            triggerSuccessFeedback();
            return true;
        } else if (event->type() == QEvent::MouseButtonPress) {
            // For testing with mouse
            triggerSuccessFeedback();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void KisFluentGestureGuide::onAnimationTick()
{
    m_animTick++;
    m_animationArea->update();
}

void KisFluentGestureGuide::triggerSuccessFeedback()
{
    if (!m_stepSuccessful) {
        m_stepSuccessful = true;
        m_feedbackLabel->setText(tr("✓ Great job!"));
        m_feedbackLabel->show();
        // Optional: Confetti particles could be triggered here via a custom widget/painter
    }
}
