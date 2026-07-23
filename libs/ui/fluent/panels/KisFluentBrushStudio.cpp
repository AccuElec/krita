// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#include "KisFluentBrushStudio.h"

#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>

KisFluentBrushStudio::KisFluentBrushStudio(QWidget *parent)
    : QWidget(parent)
    , m_slideAnimation(new QPropertyAnimation(this, "geometry", this))
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    QWidget::hide(); // Initially hidden

    setupUi();
}

KisFluentBrushStudio::~KisFluentBrushStudio()
{
}

void KisFluentBrushStudio::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(16);

    // Top area: Preview canvas (uses QPainter to render sample strokes in a real implementation)
    m_previewCanvas = new QWidget(this);
    m_previewCanvas->setMinimumHeight(200);
    m_previewCanvas->setStyleSheet("background: white; border-radius: 16px;");
    mainLayout->addWidget(m_previewCanvas);

    // Tab widget for deep customization: Stroke, Shape, Grain, Dynamics, Color, Wet Mix
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane { border: none; background: transparent; }"
        "QTabBar::tab { background: #2C2C2E; color: white; padding: 12px 24px; border-radius: 8px; margin-right: 8px; font-size: 16px; }"
        "QTabBar::tab:selected { background: #0A84FF; }"
    );
    setupTabs();
    mainLayout->addWidget(m_tabWidget, 1);

    // Bottom area: Actions
    QHBoxLayout *actionsLayout = new QHBoxLayout();
    actionsLayout->addStretch();
    
    QPushButton *btnCancel = new QPushButton(tr("Close"), this);
    btnCancel->setMinimumHeight(48);
    btnCancel->setStyleSheet("background: #3A3A3C; color: white; border-radius: 24px; padding: 0 24px; font-weight: bold;");
    connect(btnCancel, &QPushButton::clicked, this, &KisFluentBrushStudio::hide);
    
    QPushButton *btnSave = new QPushButton(tr("Save as New"), this);
    btnSave->setMinimumHeight(48);
    btnSave->setStyleSheet("background: #0A84FF; color: white; border-radius: 24px; padding: 0 24px; font-weight: bold;");
    connect(btnSave, &QPushButton::clicked, [this]() {
        saveAsNewPreset("New_Preset");
    });

    actionsLayout->addWidget(btnCancel);
    actionsLayout->addWidget(btnSave);
    mainLayout->addLayout(actionsLayout);

    // Slide-up animation from bottom
    m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_slideAnimation->setDuration(400);
}

void KisFluentBrushStudio::setupTabs()
{
    QStringList tabNames = {tr("Stroke"), tr("Shape"), tr("Grain"), tr("Dynamics"), tr("Color"), tr("Wet Mix")};
    
    for (const QString& name : tabNames) {
        QWidget *tab = new QWidget();
        QVBoxLayout *tabLayout = new QVBoxLayout(tab);
        tabLayout->setSpacing(16);

        // Add dummy sliders with custom 48dp track height
        for (int i = 0; i < 4; ++i) {
            QHBoxLayout *sliderLayout = new QHBoxLayout();
            QLabel *label = new QLabel(QString("Property %1").arg(i+1));
            label->setStyleSheet("color: white; font-size: 16px;");
            label->setFixedWidth(120);
            
            QSlider *slider = new QSlider(Qt::Horizontal);
            slider->setMinimumHeight(48); // Custom 48dp track height
            slider->setStyleSheet(
                "QSlider::groove:horizontal { border: none; height: 16px; background: #3A3A3C; border-radius: 8px; }"
                "QSlider::handle:horizontal { background: #0A84FF; width: 32px; height: 32px; border-radius: 16px; margin: -8px 0; }"
            );
            
            connect(slider, &QSlider::valueChanged, this, &KisFluentBrushStudio::presetModified);

            sliderLayout->addWidget(label);
            sliderLayout->addWidget(slider, 1);
            tabLayout->addLayout(sliderLayout);
        }
        tabLayout->addStretch();
        
        m_tabWidget->addTab(tab, name);
    }
}

void KisFluentBrushStudio::loadPreset(const QString& presetName)
{
    Q_UNUSED(presetName);
    // Load preset properties and setup preview
}

void KisFluentBrushStudio::saveAsNewPreset(const QString& name)
{
    emit presetSaved(name);
    hide();
}

void KisFluentBrushStudio::show()
{
    if (!parentWidget()) return;

    int w = parentWidget()->width();
    int h = parentWidget()->height();
    
    setGeometry(0, h, w, h);
    QWidget::show();
    raise();

    m_slideAnimation->setStartValue(QRect(0, h, w, h));
    m_slideAnimation->setEndValue(QRect(0, 0, w, h));
    m_slideAnimation->start();
}

void KisFluentBrushStudio::hide()
{
    if (!parentWidget()) return;

    int w = width();
    int h = height();
    
    m_slideAnimation->setStartValue(geometry());
    m_slideAnimation->setEndValue(QRect(0, parentWidget()->height(), w, h));
    connect(m_slideAnimation, &QPropertyAnimation::finished, this, [this]() {
        QWidget::hide();
    }, Qt::UniqueConnection);
    connect(m_slideAnimation, &QPropertyAnimation::finished, this, &KisFluentBrushStudio::closed, Qt::UniqueConnection);
    m_slideAnimation->start();
}

void KisFluentBrushStudio::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    QColor bgColor(28, 28, 30, 255);
    painter.fillRect(rect(), bgColor);
}
