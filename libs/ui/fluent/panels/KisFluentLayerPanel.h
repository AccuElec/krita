// KisFluentLayerPanel.h
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#ifndef KIS_FLUENT_LAYER_PANEL_H
#define KIS_FLUENT_LAYER_PANEL_H

#include <QWidget>
#include <QListView>
#include <QStyledItemDelegate>
#include <QPropertyAnimation>
#include <QPushButton>

class QAbstractItemModel;
class QPainter;
class QEvent;

/**
 * @brief Delegate for drawing individual layer cards in the layer panel.
 */
class KisFluentLayerDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit KisFluentLayerDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void layerVisibilityToggled(int index, bool visible);
    void layerAlphaLockToggled(int index, bool locked);
    void layerDeleted(int index);
    void opacitySliderRequested(int index);
    void blendModeDropdownRequested(int index);
};

/**
 * @brief Slide-out panel for layer management in the KritaFluent UI.
 */
class KisFluentLayerPanel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int slideOffset READ slideOffset WRITE setSlideOffset)

public:
    explicit KisFluentLayerPanel(QWidget *parent = nullptr);
    ~KisFluentLayerPanel() override;

    void setLayerModel(QAbstractItemModel *model);
    void updateThumbnails();

    int slideOffset() const;
    void setSlideOffset(int offset);

public slots:
    void show();
    void hide();

signals:
    void layerSelected(int index);
    void layerVisibilityChanged(int index, bool visible);
    void panelClosed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onNewLayerButtonClicked();
    void onAnimationFinished();

private:
    QListView *m_listView;
    KisFluentLayerDelegate *m_delegate;
    QPropertyAnimation *m_slideAnimation;
    QPushButton *m_fabButton;
    int m_slideOffset;
};

#endif // KIS_FLUENT_LAYER_PANEL_H
