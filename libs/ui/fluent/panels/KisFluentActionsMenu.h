// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KIS_FLUENT_ACTIONS_MENU_H
#define KIS_FLUENT_ACTIONS_MENU_H

#include <QWidget>
#include <QString>

class QListWidget;
class QStackedWidget;
class QPropertyAnimation;

/**
 * @brief The KisFluentActionsMenu class
 * 
 * Provides a full-screen semi-transparent overlay menu for actions.
 * Accessed via the Wrench icon on the TopBar.
 * Contains a sidebar navigation on the left and a content area on the right.
 */
class KisFluentActionsMenu : public QWidget
{
    Q_OBJECT

public:
    explicit KisFluentActionsMenu(QWidget *parent = nullptr);
    ~KisFluentActionsMenu() override;

    void showMenu();
    void hideMenu();

signals:
    void actionTriggered(const QString &actionId);
    void closed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onCategorySelected(int index);
    void onActionClicked(const QString &actionId);

private:
    void setupUi();
    QWidget* createCanvasTab();
    QWidget* createEditTab();
    QWidget* createAdjustmentsTab();
    QWidget* createViewTab();
    QWidget* createAnimationTab();
    QWidget* createPreferencesTab();
    QWidget* createHelpTab();

    QListWidget *m_sidebar;
    QStackedWidget *m_contentStack;
    QPropertyAnimation *m_slideAnimation;
};

#endif // KIS_FLUENT_ACTIONS_MENU_H
