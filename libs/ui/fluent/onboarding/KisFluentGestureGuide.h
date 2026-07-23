// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KIS_FLUENT_GESTURE_GUIDE_H
#define KIS_FLUENT_GESTURE_GUIDE_H

#include <QWidget>

class QPropertyAnimation;
class QPushButton;
class QLabel;
class QCheckBox;

/**
 * @brief The KisFluentGestureGuide class
 *
 * A full-screen overlay providing a step-by-step interactive tutorial for
 * learning KritaFluent's touch gestures. It includes 7 steps with animated
 * illustrations and an interactive practice area.
 */
class KisFluentGestureGuide : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentStep READ currentStep WRITE setCurrentStep NOTIFY currentStepChanged)
    Q_PROPERTY(int totalSteps READ totalSteps CONSTANT)

public:
    explicit KisFluentGestureGuide(QWidget *parent = nullptr);
    ~KisFluentGestureGuide() override;

    int currentStep() const;
    void setCurrentStep(int step);

    int totalSteps() const;

signals:
    void tutorialCompleted();
    void tutorialSkipped();
    void stepCompleted(int step);
    void currentStepChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void nextStep();
    void skipTutorial();
    void onAnimationTick();

private:
    void setupUi();
    void updateStepContent();
    void triggerSuccessFeedback();

    int m_currentStep {0};
    const int m_totalSteps {7};
    
    QLabel *m_titleLabel {nullptr};
    QLabel *m_subtitleLabel {nullptr};
    QWidget *m_animationArea {nullptr};
    QWidget *m_practiceArea {nullptr};
    QLabel *m_feedbackLabel {nullptr};
    QPushButton *m_nextButton {nullptr};
    QPushButton *m_skipButton {nullptr};
    QCheckBox *m_dontShowAgainCheck {nullptr};

    QWidget *m_dotsContainer {nullptr};
    
    QPropertyAnimation *m_illustrationAnimation {nullptr};
    bool m_stepSuccessful {false};
    int m_animTick {0};
};

#endif // KIS_FLUENT_GESTURE_GUIDE_H
