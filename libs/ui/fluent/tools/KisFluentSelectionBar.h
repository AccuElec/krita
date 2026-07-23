/*
 * SPDX-FileCopyrightText: 2026 KritaFluent Contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KIS_FLUENT_SELECTION_BAR_H
#define KIS_FLUENT_SELECTION_BAR_H

#include <QWidget>
#include <QPropertyAnimation>
#include "KisFluentUiExport.h"

/**
 * @brief Floating bottom toolbar for selection tools in the KritaFluent UI.
 * 
 * Provides tool mode buttons (Freehand, Rectangle, etc.), modifier buttons
 * (Add, Subtract, etc.), and actions (Invert, Deselect, etc.). Features a 
 * dark semi-transparent background, rounded corners, and slide-up animation.
 */
class KRITA_FLUENT_UI_EXPORT KisFluentSelectionBar : public QWidget
{
    Q_OBJECT

public:
    enum SelectionMode {
        ModeFreehand,
        ModeRectangle,
        ModeEllipse,
        ModePolygonal,
        ModeMagnetic,
        ModeColorRange
    };
    Q_ENUM(SelectionMode)

    enum SelectionModifier {
        ModifierReplace,
        ModifierAdd,
        ModifierSubtract,
        ModifierIntersect
    };
    Q_ENUM(SelectionModifier)

    enum SelectionAction {
        ActionInvert,
        ActionDeselect,
        ActionSelectAll,
        ActionCreateMask
    };
    Q_ENUM(SelectionAction)

    explicit KisFluentSelectionBar(QWidget *parent = nullptr);
    ~KisFluentSelectionBar() override;

    SelectionMode currentMode() const;
    SelectionModifier currentModifier() const;

public Q_SLOTS:
    void setMode(SelectionMode mode);
    void setModifier(SelectionModifier modifier);
    
    /**
     * @brief Shows the selection bar with a slide-up animation.
     */
    void showAnimated();
    
    /**
     * @brief Hides the selection bar with a slide-down animation.
     */
    void hideAnimated();

Q_SIGNALS:
    void modeChanged(SelectionMode mode);
    void modifierChanged(SelectionModifier modifier);
    void actionTriggered(SelectionAction action);
    void featherChanged(int radius);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // KIS_FLUENT_SELECTION_BAR_H
