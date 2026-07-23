/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 KritaFluent Contributors
 */

#ifndef KIS_FLUENT_TOP_BAR_H
#define KIS_FLUENT_TOP_BAR_H

#include <QWidget>
#include <QToolButton>
#include <QColor>

class QHBoxLayout;
class QAction;

/**
 * @brief Custom tool button for the Fluent Top Bar.
 */
class KisFluentToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit KisFluentToolButton(QWidget *parent = nullptr);
    ~KisFluentToolButton() override;

    void setSwatchColor(const QColor &color);
    bool isSwatch() const;
    void setIsSwatch(bool swatch);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_isSwatch;
    QColor m_swatchColor;
};

/**
 * @brief The Fluent UI Top Bar containing primary actions and tool toggles.
 */
class KisFluentTopBar : public QWidget
{
    Q_OBJECT

public:
    explicit KisFluentTopBar(QWidget *parent = nullptr);
    ~KisFluentTopBar() override;

    /**
     * @brief Enables or disables drag-and-drop reordering of buttons.
     */
    void setCustomizable(bool customizable);

    /**
     * @brief Adds a custom pinned action to the top bar.
     */
    void addPinnedAction(QAction *action);

    /**
     * @brief Updates the color swatch to reflect the current foreground color.
     */
    void updateSwatchColor(const QColor &color);

signals:
    void galleryRequested();
    void brushPanelToggled();
    void colorPanelToggled();
    void layerPanelToggled();
    void settingsRequested();

private:
    QHBoxLayout *m_mainLayout;
    QHBoxLayout *m_leftCluster;
    QHBoxLayout *m_rightCluster;
    
    KisFluentToolButton *m_galleryBtn;
    KisFluentToolButton *m_saveBtn;
    KisFluentToolButton *m_undoBtn;
    KisFluentToolButton *m_redoBtn;

    KisFluentToolButton *m_brushBtn;
    KisFluentToolButton *m_eraserBtn;
    KisFluentToolButton *m_swatchBtn;
    KisFluentToolButton *m_layersBtn;
    KisFluentToolButton *m_settingsBtn;

    bool m_customizable;
    
    void setupUI();
};

#endif // KIS_FLUENT_TOP_BAR_H
