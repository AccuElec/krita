// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KIS_FLUENT_COLOR_PANEL_H
#define KIS_FLUENT_COLOR_PANEL_H

#include <QWidget>
#include <QColor>

class QTabWidget;
class QPropertyAnimation;
class QPaintEvent;

/**
 * @class KisFluentColorPanel
 * @brief Multi-mode color picker system for KritaFluent.
 */
class KisFluentColorPanel : public QWidget
{
    Q_OBJECT

public:
    explicit KisFluentColorPanel(QWidget *parent = nullptr);
    ~KisFluentColorPanel() override;

    QColor foregroundColor() const;
    void setForegroundColor(const QColor &color);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    void showPanel();
    void hidePanel();

Q_SIGNALS:
    void foregroundColorChanged(const QColor &color);
    void backgroundColorChanged(const QColor &color);
    void panelClosed();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUi();
    
    QColor m_foregroundColor;
    QColor m_backgroundColor;
    
    QTabWidget *m_tabWidget;
    QWidget *m_discModeWidget;
    QWidget *m_classicModeWidget;
    QWidget *m_harmonyModeWidget;
    QWidget *m_valueModeWidget;
    QWidget *m_paletteModeWidget;
    
    QPropertyAnimation *m_scaleAnimation;
};

#endif // KIS_FLUENT_COLOR_PANEL_H
