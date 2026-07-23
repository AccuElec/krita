// KisFluentQuickMenu.h
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#ifndef KIS_FLUENT_QUICK_MENU_H
#define KIS_FLUENT_QUICK_MENU_H

#include <QWidget>
#include <QAction>
#include <QIcon>
#include <QPropertyAnimation>
#include <QMap>

/**
 * @brief Radial quick menu for fast tool access via touch or mouse.
 */
class KisFluentQuickMenu : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)

public:
    explicit KisFluentQuickMenu(QWidget *parent = nullptr);
    ~KisFluentQuickMenu() override;

    void setSectorAction(int sector, QAction *action, const QIcon &icon);
    
    void showAt(QPointF pos);
    void dismiss();

    int hoveredSector() const;
    int sectorCount() const;

    qreal scale() const;
    void setScale(qreal scale);

signals:
    void sectorActivated(int sectorIndex);
    void menuDismissed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    struct SectorData {
        QAction *action = nullptr;
        QIcon icon;
    };

    int calculateSector(QPointF pos) const;

    QMap<int, SectorData> m_sectors;
    int m_sectorCount;
    int m_hoveredSector;
    qreal m_scale;
    QPropertyAnimation *m_scaleAnimation;
    QPointF m_centerPos;
};

#endif // KIS_FLUENT_QUICK_MENU_H
