/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 KritaFluent Contributors
 */

#include "KisFluentTopBar.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QStyleOptionToolButton>
#include <QPaintEvent>
#include <QAction>

KisFluentToolButton::KisFluentToolButton(QWidget *parent)
    : QToolButton(parent)
    , m_isSwatch(false)
    , m_swatchColor(Qt::black)
{
    setMinimumSize(48, 48);
    setIconSize(QSize(24, 24));
    setStyleSheet("QToolButton { border: none; border-radius: 24px; background: transparent; }"
                  "QToolButton:hover { background: rgba(255, 255, 255, 0.1); }"
                  "QToolButton:pressed { background: rgba(255, 255, 255, 0.2); }");
}

KisFluentToolButton::~KisFluentToolButton()
{
}

void KisFluentToolButton::setSwatchColor(const QColor &color)
{
    m_swatchColor = color;
    update();
}

bool KisFluentToolButton::isSwatch() const
{
    return m_isSwatch;
}

void KisFluentToolButton::setIsSwatch(bool swatch)
{
    m_isSwatch = swatch;
    update();
}

void KisFluentToolButton::paintEvent(QPaintEvent *event)
{
    if (m_isSwatch) {
        // Custom painting for the color swatch
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw hover/pressed background
        if (isDown()) {
            painter.setBrush(QColor(255, 255, 255, 51));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(rect());
        } else if (underMouse()) {
            painter.setBrush(QColor(255, 255, 255, 25));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(rect());
        }

        // Draw swatch circle
        QRectF swatchRect(12, 12, 24, 24);
        painter.setBrush(m_swatchColor);
        painter.setPen(QPen(QColor(255, 255, 255, 100), 2));
        painter.drawEllipse(swatchRect);
    } else {
        QToolButton::paintEvent(event);
    }
}


KisFluentTopBar::KisFluentTopBar(QWidget *parent)
    : QWidget(parent)
    , m_customizable(false)
{
    setFixedHeight(48);
    // Dark semi-transparent background (#1C1C1E at 90% opacity)
    setStyleSheet("KisFluentTopBar { background-color: rgba(28, 28, 30, 0.9); }");
    
    setupUI();
}

KisFluentTopBar::~KisFluentTopBar()
{
}

void KisFluentTopBar::setupUI()
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 0, 8, 0);
    m_mainLayout->setSpacing(0);

    m_leftCluster = new QHBoxLayout();
    m_leftCluster->setSpacing(4);
    
    m_galleryBtn = new KisFluentToolButton(this);
    m_galleryBtn->setIcon(QIcon::fromTheme("go-previous")); // Temp icon
    m_saveBtn = new KisFluentToolButton(this);
    m_saveBtn->setIcon(QIcon::fromTheme("document-save"));
    m_undoBtn = new KisFluentToolButton(this);
    m_undoBtn->setIcon(QIcon::fromTheme("edit-undo"));
    m_redoBtn = new KisFluentToolButton(this);
    m_redoBtn->setIcon(QIcon::fromTheme("edit-redo"));

    m_leftCluster->addWidget(m_galleryBtn);
    m_leftCluster->addWidget(m_saveBtn);
    m_leftCluster->addWidget(m_undoBtn);
    m_leftCluster->addWidget(m_redoBtn);

    m_mainLayout->addLayout(m_leftCluster);
    m_mainLayout->addStretch();

    m_rightCluster = new QHBoxLayout();
    m_rightCluster->setSpacing(4);

    m_brushBtn = new KisFluentToolButton(this);
    m_brushBtn->setIcon(QIcon::fromTheme("brush"));
    m_eraserBtn = new KisFluentToolButton(this);
    m_eraserBtn->setIcon(QIcon::fromTheme("draw-eraser"));
    m_swatchBtn = new KisFluentToolButton(this);
    m_swatchBtn->setIsSwatch(true);
    m_layersBtn = new KisFluentToolButton(this);
    m_layersBtn->setIcon(QIcon::fromTheme("dialog-information")); // Temp layers icon
    m_settingsBtn = new KisFluentToolButton(this);
    m_settingsBtn->setIcon(QIcon::fromTheme("configure"));

    m_rightCluster->addWidget(m_brushBtn);
    m_rightCluster->addWidget(m_eraserBtn);
    m_rightCluster->addWidget(m_swatchBtn);
    m_rightCluster->addWidget(m_layersBtn);
    m_rightCluster->addWidget(m_settingsBtn);

    m_mainLayout->addLayout(m_rightCluster);

    // Connect signals
    connect(m_galleryBtn, &QToolButton::clicked, this, &KisFluentTopBar::galleryRequested);
    connect(m_brushBtn, &QToolButton::clicked, this, &KisFluentTopBar::brushPanelToggled);
    connect(m_swatchBtn, &QToolButton::clicked, this, &KisFluentTopBar::colorPanelToggled);
    connect(m_layersBtn, &QToolButton::clicked, this, &KisFluentTopBar::layerPanelToggled);
    connect(m_settingsBtn, &QToolButton::clicked, this, &KisFluentTopBar::settingsRequested);
}

void KisFluentTopBar::setCustomizable(bool customizable)
{
    m_customizable = customizable;
    // Implement drag-and-drop logic enabling/disabling
}

void KisFluentTopBar::addPinnedAction(QAction *action)
{
    if (!action) return;
    KisFluentToolButton *btn = new KisFluentToolButton(this);
    btn->setDefaultAction(action);
    m_rightCluster->insertWidget(0, btn);
}

void KisFluentTopBar::updateSwatchColor(const QColor &color)
{
    m_swatchBtn->setSwatchColor(color);
}
