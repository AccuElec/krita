// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "KisFluentActionsMenu.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>

KisFluentActionsMenu::KisFluentActionsMenu(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    
    // Setup animation
    m_slideAnimation = new QPropertyAnimation(this, "geometry", this);
    m_slideAnimation->setDuration(300);
    m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    hide();
}

KisFluentActionsMenu::~KisFluentActionsMenu()
{
}

void KisFluentActionsMenu::setupUi()
{
    // Full screen overlay style
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("KisFluentActionsMenu { background-color: rgba(0, 0, 0, 180); }");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50);

    // Inner container for the menu
    QWidget *menuContainer = new QWidget(this);
    menuContainer->setObjectName("MenuContainer");
    menuContainer->setStyleSheet(
        "#MenuContainer {"
        "  background-color: #2c2c2c;"
        "  border-radius: 12px;"
        "}"
    );

    QHBoxLayout *containerLayout = new QHBoxLayout(menuContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    // Sidebar
    m_sidebar = new QListWidget(menuContainer);
    m_sidebar->setFixedWidth(200);
    m_sidebar->setStyleSheet(
        "QListWidget {"
        "  background-color: #202020;"
        "  border-top-left-radius: 12px;"
        "  border-bottom-left-radius: 12px;"
        "  border: none;"
        "  padding: 10px;"
        "}"
        "QListWidget::item {"
        "  color: white;"
        "  padding: 15px;"
        "  border-radius: 8px;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #3a3a3a;"
        "}"
    );

    QStringList categories = {"Canvas", "Edit", "Adjustments", "View", "Animation", "Preferences", "Help"};
    m_sidebar->addItems(categories);

    // Content Stack
    m_contentStack = new QStackedWidget(menuContainer);
    m_contentStack->setStyleSheet("background-color: transparent;");

    m_contentStack->addWidget(createCanvasTab());
    m_contentStack->addWidget(createEditTab());
    m_contentStack->addWidget(createAdjustmentsTab());
    m_contentStack->addWidget(createViewTab());
    m_contentStack->addWidget(createAnimationTab());
    m_contentStack->addWidget(createPreferencesTab());
    m_contentStack->addWidget(createHelpTab());

    containerLayout->addWidget(m_sidebar);
    containerLayout->addWidget(m_contentStack);

    mainLayout->addWidget(menuContainer);

    connect(m_sidebar, &QListWidget::currentRowChanged, this, &KisFluentActionsMenu::onCategorySelected);
    
    if (m_sidebar->count() > 0) {
        m_sidebar->setCurrentRow(0);
    }
}

QWidget* KisFluentActionsMenu::createCanvasTab()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    QStringList actions = {"New", "Open", "Save", "Save As", "Export PNG", "Export PSD", "Export JPG", "Export KRA", "Canvas Size", "Image Size"};
    for (const QString &a : actions) {
        QPushButton *btn = new QPushButton(a, w);
        connect(btn, &QPushButton::clicked, this, [this, a](){ onActionClicked("canvas_" + a); });
        l->addWidget(btn);
    }
    l->addStretch();
    return w;
}

QWidget* KisFluentActionsMenu::createEditTab()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    QStringList actions = {"Cut", "Copy", "Paste", "Clear", "Crop", "Fill"};
    for (const QString &a : actions) {
        QPushButton *btn = new QPushButton(a, w);
        connect(btn, &QPushButton::clicked, this, [this, a](){ onActionClicked("edit_" + a); });
        l->addWidget(btn);
    }
    l->addStretch();
    return w;
}

QWidget* KisFluentActionsMenu::createAdjustmentsTab()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    QStringList actions = {"HSV Adjustment", "Curves", "Levels", "Color Balance", "Desaturate", "Blur", "Sharpen"};
    for (const QString &a : actions) {
        QPushButton *btn = new QPushButton(a, w);
        connect(btn, &QPushButton::clicked, this, [this, a](){ onActionClicked("adj_" + a); });
        l->addWidget(btn);
    }
    l->addStretch();
    return w;
}

QWidget* KisFluentActionsMenu::createViewTab()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    QStringList actions = {"Flip Horizontal", "Flip Vertical", "Toggle Grid", "Toggle Guides", "Reference Images"};
    for (const QString &a : actions) {
        QPushButton *btn = new QPushButton(a, w);
        connect(btn, &QPushButton::clicked, this, [this, a](){ onActionClicked("view_" + a); });
        l->addWidget(btn);
    }
    l->addStretch();
    return w;
}

QWidget* KisFluentActionsMenu::createAnimationTab()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    QStringList actions = {"Toggle Timeline", "Onion Skin Settings", "FPS Selector"};
    for (const QString &a : actions) {
        QPushButton *btn = new QPushButton(a, w);
        connect(btn, &QPushButton::clicked, this, [this, a](){ onActionClicked("anim_" + a); });
        l->addWidget(btn);
    }
    l->addStretch();
    return w;
}

QWidget* KisFluentActionsMenu::createPreferencesTab()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    QStringList actions = {"Touch Gestures config", "Handedness", "Theme", "Brush Cursor mode"};
    for (const QString &a : actions) {
        QPushButton *btn = new QPushButton(a, w);
        connect(btn, &QPushButton::clicked, this, [this, a](){ onActionClicked("pref_" + a); });
        l->addWidget(btn);
    }
    l->addStretch();
    return w;
}

QWidget* KisFluentActionsMenu::createHelpTab()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    QStringList actions = {"Gesture Guide", "Shortcuts Overview", "About KritaFluent"};
    for (const QString &a : actions) {
        QPushButton *btn = new QPushButton(a, w);
        connect(btn, &QPushButton::clicked, this, [this, a](){ onActionClicked("help_" + a); });
        l->addWidget(btn);
    }
    l->addStretch();
    return w;
}

void KisFluentActionsMenu::onCategorySelected(int index)
{
    if (index >= 0 && index < m_contentStack->count()) {
        m_contentStack->setCurrentIndex(index);
    }
}

void KisFluentActionsMenu::onActionClicked(const QString &actionId)
{
    emit actionTriggered(actionId);
    hideMenu();
}

void KisFluentActionsMenu::showMenu()
{
    if (!parentWidget()) return;
    
    QRect parentRect = parentWidget()->rect();
    
    // Slide down from top
    QRect startRect = parentRect;
    startRect.moveTop(-parentRect.height());
    
    setGeometry(startRect);
    show();
    raise();
    
    m_slideAnimation->setStartValue(startRect);
    m_slideAnimation->setEndValue(parentRect);
    m_slideAnimation->start();
}

void KisFluentActionsMenu::hideMenu()
{
    if (!parentWidget()) {
        hide();
        emit closed();
        return;
    }
    
    QRect parentRect = parentWidget()->rect();
    QRect endRect = parentRect;
    endRect.moveTop(-parentRect.height());
    
    m_slideAnimation->setStartValue(geometry());
    m_slideAnimation->setEndValue(endRect);
    
    connect(m_slideAnimation, &QPropertyAnimation::finished, this, [this](){
        hide();
        emit closed();
        m_slideAnimation->disconnect(this);
    });
    
    m_slideAnimation->start();
}

void KisFluentActionsMenu::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void KisFluentActionsMenu::mousePressEvent(QMouseEvent *event)
{
    // Close menu if clicked outside the main container
    QWidget *menuContainer = findChild<QWidget*>("MenuContainer");
    if (menuContainer && !menuContainer->geometry().contains(event->pos())) {
        hideMenu();
    } else {
        QWidget::mousePressEvent(event);
    }
}
