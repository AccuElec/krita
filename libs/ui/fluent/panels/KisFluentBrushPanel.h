// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#ifndef KIS_FLUENT_BRUSH_PANEL_H
#define KIS_FLUENT_BRUSH_PANEL_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QListWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QStringList>
#include <QScrollArea>
#include <QLabel>
#include <QMap>

// Forward declarations
class KisPaintOpPreset;
typedef QSharedPointer<KisPaintOpPreset> KisPaintOpPresetSP;

/**
 * @brief Slide-out brush library and brush studio panel for KritaFluent
 * 
 * Provides a touch-friendly interface for selecting and organizing brush presets.
 * Features a two-column layout with categories and brush thumbnails.
 */
class KisFluentBrushPanel : public QWidget
{
    Q_OBJECT

public:
    explicit KisFluentBrushPanel(QWidget *parent = nullptr);
    ~KisFluentBrushPanel() override;

    void setBrushPresets(const QList<KisPaintOpPresetSP>& presets);
    void setCategories(const QStringList& categories);
    void selectBrush(const QString& presetName);

public slots:
    void show();
    void hide();

signals:
    void brushSelected(const QString& presetName);
    void brushStudioRequested(const QString& presetName);
    void panelClosed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onCategorySelected(QListWidgetItem *item);
    void onBrushThumbnailClicked(const QString& presetName);

private:
    void setupUi();
    void updateBrushGrid(const QString& category, const QString& filterText = QString());

    QPropertyAnimation *m_slideAnimation;
    QLineEdit *m_searchBar;
    QListWidget *m_categoryList;
    QWidget *m_brushGridContainer;
    QGridLayout *m_brushGridLayout;
    QScrollArea *m_gridScrollArea;
    QWidget *m_recentArea;
    QWidget *m_pinnedArea;

    QList<KisPaintOpPresetSP> m_allPresets;
    QString m_currentCategory;
    QString m_currentSearchText;
};

#endif // KIS_FLUENT_BRUSH_PANEL_H
