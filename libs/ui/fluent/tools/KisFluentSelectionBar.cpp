/*
 * SPDX-FileCopyrightText: 2026 KritaFluent Contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KisFluentSelectionBar.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QToolButton>
#include <QButtonGroup>
#include <QStyleOption>
#include <QIcon>
#include <QEvent>
#include <QToolTip>

class KisFluentSelectionBar::Private
{
public:
    SelectionMode mode {ModeFreehand};
    SelectionModifier modifier {ModifierReplace};
    QPropertyAnimation *animation {nullptr};
    
    QButtonGroup *modeGroup {nullptr};
    QButtonGroup *modifierGroup {nullptr};
};

KisFluentSelectionBar::KisFluentSelectionBar(QWidget *parent)
    : QWidget(parent)
    , d(new Private())
{
    setFixedHeight(56);
    
    d->modeGroup = new QButtonGroup(this);
    d->modifierGroup = new QButtonGroup(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(4);

    // Modes Placeholder
    QToolButton *btnFreehand = new QToolButton(this);
    btnFreehand->setCheckable(true);
    d->modeGroup->addButton(btnFreehand, ModeFreehand);
    layout->addWidget(btnFreehand);

    d->animation = new QPropertyAnimation(this, "pos", this);
    d->animation->setDuration(300);
    d->animation->setEasingCurve(QEasingCurve::OutCubic);
    
    connect(d->modeGroup, QOverload<int>::of(&QButtonGroup::idClicked), this, [this](int id) {
        setMode(static_cast<SelectionMode>(id));
    });

    connect(d->modifierGroup, QOverload<int>::of(&QButtonGroup::idClicked), this, [this](int id) {
        setModifier(static_cast<SelectionModifier>(id));
    });
}

KisFluentSelectionBar::~KisFluentSelectionBar()
{
}

KisFluentSelectionBar::SelectionMode KisFluentSelectionBar::currentMode() const
{
    return d->mode;
}

KisFluentSelectionBar::SelectionModifier KisFluentSelectionBar::currentModifier() const
{
    return d->modifier;
}

void KisFluentSelectionBar::setMode(SelectionMode mode)
{
    if (d->mode != mode) {
        d->mode = mode;
        Q_EMIT modeChanged(mode);
    }
}

void KisFluentSelectionBar::setModifier(SelectionModifier modifier)
{
    if (d->modifier != modifier) {
        d->modifier = modifier;
        Q_EMIT modifierChanged(modifier);
    }
}

void KisFluentSelectionBar::showAnimated()
{
    if (!parentWidget()) return;
    show();
    
    int startY = parentWidget()->height();
    int endY = parentWidget()->height() - height() - 16;
    
    d->animation->setStartValue(QPoint(pos().x(), startY));
    d->animation->setEndValue(QPoint(pos().x(), endY));
    d->animation->start();
}

void KisFluentSelectionBar::hideAnimated()
{
    if (!parentWidget()) {
        hide();
        return;
    }
    
    int endY = parentWidget()->height();
    
    d->animation->setStartValue(pos());
    d->animation->setEndValue(QPoint(pos().x(), endY));
    
    connect(d->animation, &QPropertyAnimation::finished, this, &QWidget::hide, Qt::UniqueConnection);
    d->animation->start();
}

void KisFluentSelectionBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 12, 12);

    painter.fillPath(path, QColor(30, 30, 30, 200));
}

void KisFluentSelectionBar::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (parentWidget()) {
        move((parentWidget()->width() - width()) / 2, pos().y());
    }
}
