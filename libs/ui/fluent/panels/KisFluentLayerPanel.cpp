// KisFluentLayerPanel.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#include "KisFluentLayerPanel.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QMenu>

KisFluentLayerDelegate::KisFluentLayerDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void KisFluentLayerDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    
    QRect rect = option.rect;
    painter->setRenderHint(QPainter::Antialiasing);

    // Background
    if (option.state & QStyle::State_Selected) {
        painter->setBrush(QColor("#3A3A3C"));
    } else {
        painter->setBrush(QColor("#2C2C2E"));
    }
    painter->setPen(QPen(QColor("#48484A"), 1));
    painter->drawRoundedRect(rect.adjusted(4, 4, -4, -4), 8, 8);

    // Thumbnail area (48x48dp)
    QRect thumbRect(rect.left() + 12, rect.top() + 8, 48, 48);
    painter->setBrush(Qt::darkGray);
    painter->drawRoundedRect(thumbRect, 4, 4);

    // Name
    painter->setPen(Qt::white);
    painter->drawText(QRect(rect.left() + 72, rect.top() + 8, rect.width() - 140, 24), Qt::AlignLeft | Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

    // Icons: Visibility, Lock, Blend mode
    painter->drawText(QRect(rect.right() - 80, rect.top() + 8, 24, 48), Qt::AlignCenter, "N"); // Blend Mode indicator
    painter->drawText(QRect(rect.right() - 50, rect.top() + 8, 20, 48), Qt::AlignCenter, "L"); // Lock icon placeholder
    painter->drawText(QRect(rect.right() - 25, rect.top() + 8, 20, 48), Qt::AlignCenter, "V"); // Visibility icon placeholder

    painter->restore();
}

QSize KisFluentLayerDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(200, 64); // 64dp height
}

bool KisFluentLayerDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // Implementation for gestures (swipe right/left, two-finger tap, long-press) would go here
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

KisFluentLayerPanel::KisFluentLayerPanel(QWidget *parent)
    : QWidget(parent),
      m_slideOffset(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    
    m_listView = new QListView(this);
    m_delegate = new KisFluentLayerDelegate(this);
    m_listView->setItemDelegate(m_delegate);
    m_listView->setStyleSheet("QListView { background: transparent; border: none; outline: none; }");
    m_listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    m_fabButton = new QPushButton("+", this);
    m_fabButton->setFixedSize(56, 56);
    m_fabButton->setStyleSheet("QPushButton { background-color: #5E9CFF; color: white; border-radius: 28px; font-size: 24px; }");
    connect(m_fabButton, &QPushButton::clicked, this, &KisFluentLayerPanel::onNewLayerButtonClicked);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_listView);
    layout->setContentsMargins(0, 0, 0, 0);

    m_slideAnimation = new QPropertyAnimation(this, "slideOffset", this);
    m_slideAnimation->setDuration(300);
    m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_slideAnimation, &QPropertyAnimation::finished, this, &KisFluentLayerPanel::onAnimationFinished);

    connect(m_delegate, &KisFluentLayerDelegate::layerVisibilityToggled, this, &KisFluentLayerPanel::layerVisibilityChanged);
}

KisFluentLayerPanel::~KisFluentLayerPanel() = default;

void KisFluentLayerPanel::setLayerModel(QAbstractItemModel *model)
{
    m_listView->setModel(model);
}

void KisFluentLayerPanel::updateThumbnails()
{
    m_listView->viewport()->update();
}

int KisFluentLayerPanel::slideOffset() const
{
    return m_slideOffset;
}

void KisFluentLayerPanel::setSlideOffset(int offset)
{
    m_slideOffset = offset;
    update();
}

void KisFluentLayerPanel::show()
{
    QWidget::show();
    if (parentWidget()) {
        int targetWidth = parentWidget()->width() * 0.35; // 35% parent width
        resize(targetWidth, parentWidget()->height());
        move(parentWidget()->width() - targetWidth, 0);
        
        m_slideAnimation->setStartValue(targetWidth);
        m_slideAnimation->setEndValue(0);
        m_slideAnimation->start();
    }
}

void KisFluentLayerPanel::hide()
{
    if (parentWidget()) {
        int targetWidth = width();
        m_slideAnimation->setStartValue(0);
        m_slideAnimation->setEndValue(targetWidth);
        m_slideAnimation->start();
    } else {
        QWidget::hide();
    }
}

void KisFluentLayerPanel::onAnimationFinished()
{
    if (m_slideOffset > 0) {
        QWidget::hide();
        emit panelClosed();
    }
}

void KisFluentLayerPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(m_slideOffset, 0);

    QColor bgColor("#1C1C1E"); // Dark background
    bgColor.setAlphaF(0.92); // 92% opacity
    painter.fillRect(rect(), bgColor);
}

void KisFluentLayerPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_fabButton->move(width() - 72, height() - 72);
}

void KisFluentLayerPanel::onNewLayerButtonClicked()
{
    QMenu menu(this);
    menu.addAction("New Layer");
    menu.addAction("New Group");
    menu.addAction("Duplicate Active");
    menu.exec(m_fabButton->mapToGlobal(QPoint(0, -m_fabButton->height())));
}
