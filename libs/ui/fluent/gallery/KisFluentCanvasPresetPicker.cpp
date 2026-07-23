// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#include "KisFluentCanvasPresetPicker.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QRadioButton>
#include <QPainter>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

struct KisFluentCanvasPresetPicker::Private {
    QSpinBox *widthInput;
    QSpinBox *heightInput;
    QComboBox *unitSwitcher;
    QComboBox *colorProfile;
    QSpinBox *dpiInput;
    QRadioButton *bgWhite;
    QRadioButton *bgBlack;
    QRadioButton *bgTransparent;
};

KisFluentCanvasPresetPicker::KisFluentCanvasPresetPicker(QWidget *parent)
    : QWidget(parent), d(new Private())
{
    // Semi-transparent dark overlay
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0, 0, 0, 180));
    setPalette(pal);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    // Modal Container
    QWidget *container = new QWidget(this);
    container->setFixedSize(800, 600);
    container->setStyleSheet("QWidget { background: #2C2C2E; border-radius: 20px; color: white; }");
    
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(30, 30, 30, 30);

    // Title
    QLabel *titleLabel = new QLabel(tr("Create New Canvas"), container);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    containerLayout->addWidget(titleLabel);

    // Presets Grid (stubbed)
    QWidget *presetsWidget = new QWidget(container);
    QHBoxLayout *presetsLayout = new QHBoxLayout(presetsWidget);
    // Add Preset Buttons here... Screen Size, A4, Square, 4K, Comic, Animation
    containerLayout->addWidget(presetsWidget);

    // Custom Size Section
    QHBoxLayout *customSizeLayout = new QHBoxLayout();
    d->widthInput = new QSpinBox(container);
    d->widthInput->setRange(1, 100000);
    d->heightInput = new QSpinBox(container);
    d->heightInput->setRange(1, 100000);
    d->unitSwitcher = new QComboBox(container);
    d->unitSwitcher->addItems({"px", "in", "cm", "mm"});
    
    customSizeLayout->addWidget(new QLabel(tr("Width:"), container));
    customSizeLayout->addWidget(d->widthInput);
    customSizeLayout->addWidget(new QLabel(tr("Height:"), container));
    customSizeLayout->addWidget(d->heightInput);
    customSizeLayout->addWidget(d->unitSwitcher);
    containerLayout->addLayout(customSizeLayout);

    // DPI & Color Profile
    QHBoxLayout *techLayout = new QHBoxLayout();
    d->dpiInput = new QSpinBox(container);
    d->dpiInput->setRange(1, 1200);
    d->dpiInput->setValue(300);
    d->colorProfile = new QComboBox(container);
    d->colorProfile->addItems({"sRGB", "Adobe RGB", "CMYK"});
    
    techLayout->addWidget(new QLabel(tr("DPI:"), container));
    techLayout->addWidget(d->dpiInput);
    techLayout->addWidget(new QLabel(tr("Color Profile:"), container));
    techLayout->addWidget(d->colorProfile);
    containerLayout->addLayout(techLayout);

    // Background
    QHBoxLayout *bgLayout = new QHBoxLayout();
    d->bgWhite = new QRadioButton(tr("White"), container);
    d->bgBlack = new QRadioButton(tr("Black"), container);
    d->bgTransparent = new QRadioButton(tr("Transparent"), container);
    d->bgWhite->setChecked(true);
    
    bgLayout->addWidget(new QLabel(tr("Background:"), container));
    bgLayout->addWidget(d->bgWhite);
    bgLayout->addWidget(d->bgBlack);
    bgLayout->addWidget(d->bgTransparent);
    containerLayout->addLayout(bgLayout);

    // Buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *btnCancel = new QPushButton(tr("Cancel"), container);
    QPushButton *btnCreate = new QPushButton(tr("Create"), container);
    btnCreate->setStyleSheet("background: #0078D7; color: white; border-radius: 10px; font-weight: bold; padding: 10px;");
    
    connect(btnCancel, &QPushButton::clicked, this, &KisFluentCanvasPresetPicker::cancelled);
    connect(btnCreate, &QPushButton::clicked, [this]() {
        QColor bgColor = Qt::white;
        if (d->bgBlack->isChecked()) bgColor = Qt::black;
        else if (d->bgTransparent->isChecked()) bgColor = Qt::transparent;
        
        emit canvasCreated(d->widthInput->value(), d->heightInput->value(), 
                           d->dpiInput->value(), d->colorProfile->currentText(), bgColor);
    });

    buttonsLayout->addStretch();
    buttonsLayout->addWidget(btnCancel);
    buttonsLayout->addWidget(btnCreate);
    containerLayout->addLayout(buttonsLayout);

    mainLayout->addWidget(container);
}

KisFluentCanvasPresetPicker::~KisFluentCanvasPresetPicker()
{
}

void KisFluentCanvasPresetPicker::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    // Scale-up animation
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(250);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::OutBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void KisFluentCanvasPresetPicker::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}
