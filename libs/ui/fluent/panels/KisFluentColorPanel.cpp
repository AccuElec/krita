// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "KisFluentColorPanel.h"

#include <QPainter>
#include <QPaintEvent>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QLineEdit>

KisFluentColorPanel::KisFluentColorPanel(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(350, 450);

    setupUi();
    
    m_scaleAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    m_scaleAnimation->setDuration(200);
}

KisFluentColorPanel::~KisFluentColorPanel() = default;

void KisFluentColorPanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QWidget *container = new QWidget(this);
    container->setObjectName("ColorPanelContainer");
    container->setStyleSheet(
        "#ColorPanelContainer {"
        "  background-color: rgba(28, 28, 30, 0.95);"
        "  border-radius: 12px;"
        "}"
    );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 100));
    shadow->setOffset(0, 4);
    container->setGraphicsEffect(shadow);

    QVBoxLayout *containerLayout = new QVBoxLayout(container);

    m_tabWidget = new QTabWidget(this);
    
    // Setup mode widgets
    m_discModeWidget = new QWidget(this);
    m_classicModeWidget = new QWidget(this);
    m_harmonyModeWidget = new QWidget(this);
    m_valueModeWidget = new QWidget(this);
    m_paletteModeWidget = new QWidget(this);

    // Mockup for Disc Mode
    QVBoxLayout *discLayout = new QVBoxLayout(m_discModeWidget);
    discLayout->addWidget(new QLabel("Disc Mode Area", m_discModeWidget));

    // Mockup for Classic Mode
    QVBoxLayout *classicLayout = new QVBoxLayout(m_classicModeWidget);
    classicLayout->addWidget(new QLabel("Classic Mode Area", m_classicModeWidget));

    // Mockup for Harmony Mode
    QVBoxLayout *harmonyLayout = new QVBoxLayout(m_harmonyModeWidget);
    harmonyLayout->addWidget(new QLabel("Harmony Mode Area", m_harmonyModeWidget));

    // Mockup for Value Mode
    QVBoxLayout *valueLayout = new QVBoxLayout(m_valueModeWidget);
    valueLayout->addWidget(new QLabel("Value Mode Area", m_valueModeWidget));

    // Mockup for Palette Mode
    QVBoxLayout *paletteLayout = new QVBoxLayout(m_paletteModeWidget);
    paletteLayout->addWidget(new QLabel("Palette Mode Area", m_paletteModeWidget));

    m_tabWidget->addTab(m_discModeWidget, "Disc");
    m_tabWidget->addTab(m_classicModeWidget, "Classic");
    m_tabWidget->addTab(m_harmonyModeWidget, "Harmony");
    m_tabWidget->addTab(m_valueModeWidget, "Value");
    m_tabWidget->addTab(m_paletteModeWidget, "Palette");

    containerLayout->addWidget(m_tabWidget);
    mainLayout->addWidget(container);
}

void KisFluentColorPanel::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

QColor KisFluentColorPanel::foregroundColor() const { return m_foregroundColor; }

void KisFluentColorPanel::setForegroundColor(const QColor &color)
{
    if (m_foregroundColor != color) {
        m_foregroundColor = color;
        Q_EMIT foregroundColorChanged(color);
    }
}

QColor KisFluentColorPanel::backgroundColor() const { return m_backgroundColor; }

void KisFluentColorPanel::setBackgroundColor(const QColor &color)
{
    if (m_backgroundColor != color) {
        m_backgroundColor = color;
        Q_EMIT backgroundColorChanged(color);
    }
}

void KisFluentColorPanel::showPanel()
{
    show();
    m_scaleAnimation->setStartValue(0.0);
    m_scaleAnimation->setEndValue(1.0);
    m_scaleAnimation->start();
}

void KisFluentColorPanel::hidePanel()
{
    m_scaleAnimation->setStartValue(1.0);
    m_scaleAnimation->setEndValue(0.0);
    connect(m_scaleAnimation, &QPropertyAnimation::finished, this, [this](){
        hide();
        Q_EMIT panelClosed();
        disconnect(m_scaleAnimation, &QPropertyAnimation::finished, this, nullptr);
    });
    m_scaleAnimation->start();
}
