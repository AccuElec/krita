// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#include "KisFluentBrushPanel.h"

#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QTouchEvent>
#include <QGestureEvent>
#include <QPushButton>

KisFluentBrushPanel::KisFluentBrushPanel(QWidget *parent)
    : QWidget(parent)
    , m_slideAnimation(new QPropertyAnimation(this, "geometry", this))
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    QWidget::hide(); // initially hidden

    setupUi();
}

KisFluentBrushPanel::~KisFluentBrushPanel()
{
}

void KisFluentBrushPanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // Search bar
    m_searchBar = new QLineEdit(this);
    m_searchBar->setPlaceholderText(tr("Search brushes..."));
    m_searchBar->setMinimumHeight(48); // Touch friendly
    m_searchBar->setStyleSheet("QLineEdit { background: #2C2C2E; color: white; border-radius: 24px; padding: 0 16px; font-size: 16px; }");
    connect(m_searchBar, &QLineEdit::textChanged, this, &KisFluentBrushPanel::onSearchTextChanged);
    mainLayout->addWidget(m_searchBar);

    // Two-column layout
    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(16);

    // Left column: Category list
    m_categoryList = new QListWidget(this);
    m_categoryList->setFixedWidth(120);
    m_categoryList->setStyleSheet("QListWidget { background: transparent; border: none; color: white; }"
                                  "QListWidget::item { padding: 12px; border-radius: 8px; }"
                                  "QListWidget::item:selected { background: #0A84FF; }");
    connect(m_categoryList, &QListWidget::itemClicked, this, &KisFluentBrushPanel::onCategorySelected);
    columnsLayout->addWidget(m_categoryList);

    // Right area: Brush grid
    m_gridScrollArea = new QScrollArea(this);
    m_gridScrollArea->setWidgetResizable(true);
    m_gridScrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    m_brushGridContainer = new QWidget(m_gridScrollArea);
    m_brushGridContainer->setStyleSheet("background: transparent;");
    m_brushGridLayout = new QGridLayout(m_brushGridContainer);
    m_brushGridLayout->setSpacing(8);
    m_brushGridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    
    m_gridScrollArea->setWidget(m_brushGridContainer);
    columnsLayout->addWidget(m_gridScrollArea, 1);

    mainLayout->addLayout(columnsLayout, 1);

    // Bottom area: Recent and Pinned favorites
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    m_recentArea = new QWidget(this);
    m_recentArea->setMinimumHeight(80);
    m_recentArea->setStyleSheet("background: #2C2C2E; border-radius: 12px;");
    QLabel *recentLabel = new QLabel(tr("Recent"), m_recentArea);
    recentLabel->setStyleSheet("color: white; padding: 8px;");

    m_pinnedArea = new QWidget(this);
    m_pinnedArea->setMinimumHeight(80);
    m_pinnedArea->setStyleSheet("background: #2C2C2E; border-radius: 12px;");
    QLabel *pinnedLabel = new QLabel(tr("Pinned Favorites"), m_pinnedArea);
    pinnedLabel->setStyleSheet("color: white; padding: 8px;");

    bottomLayout->addWidget(m_recentArea, 1);
    bottomLayout->addWidget(m_pinnedArea, 1);
    
    mainLayout->addLayout(bottomLayout);

    // Animation setup
    m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_slideAnimation->setDuration(300);
}

void KisFluentBrushPanel::setBrushPresets(const QList<KisPaintOpPresetSP>& presets)
{
    m_allPresets = presets;
    updateBrushGrid(m_currentCategory, m_currentSearchText);
}

void KisFluentBrushPanel::setCategories(const QStringList& categories)
{
    m_categoryList->clear();
    for (const QString& category : categories) {
        m_categoryList->addItem(category);
    }
    if (m_categoryList->count() > 0) {
        m_categoryList->setCurrentRow(0);
        m_currentCategory = categories.first();
    }
}

void KisFluentBrushPanel::selectBrush(const QString& presetName)
{
    emit brushSelected(presetName);
}

void KisFluentBrushPanel::show()
{
    if (!parentWidget()) return;
    
    // Width: 40% of parent width, full height
    int w = parentWidget()->width() * 0.4;
    int h = parentWidget()->height();
    
    setGeometry(parentWidget()->width(), 0, w, h);
    QWidget::show();
    raise();

    m_slideAnimation->setStartValue(QRect(parentWidget()->width(), 0, w, h));
    m_slideAnimation->setEndValue(QRect(parentWidget()->width() - w, 0, w, h));
    m_slideAnimation->start();
}

void KisFluentBrushPanel::hide()
{
    if (!parentWidget()) return;
    
    int w = width();
    int h = height();
    
    m_slideAnimation->setStartValue(geometry());
    m_slideAnimation->setEndValue(QRect(parentWidget()->width(), 0, w, h));
    connect(m_slideAnimation, &QPropertyAnimation::finished, this, [this]() {
        QWidget::hide();
    }, Qt::UniqueConnection);
    connect(m_slideAnimation, &QPropertyAnimation::finished, this, &KisFluentBrushPanel::panelClosed, Qt::UniqueConnection);
    m_slideAnimation->start();
}

void KisFluentBrushPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Dark semi-transparent background (#1C1C1E at 92% opacity)
    QColor bgColor(28, 28, 30, int(255 * 0.92));
    
    QPainterPath path;
    // Round corners on the left side
    path.addRoundedRect(rect(), 24.0, 24.0);
    
    painter.fillPath(path, bgColor);
}

void KisFluentBrushPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

bool KisFluentBrushPanel::eventFilter(QObject *obj, QEvent *event)
{
    // Touch gestures: Tap brush to select, long-press for preview popup, swipe left to pin/unpin
    if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd) {
        // Advanced gesture handling to be implemented
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void KisFluentBrushPanel::onSearchTextChanged(const QString& text)
{
    m_currentSearchText = text;
    updateBrushGrid(m_currentCategory, m_currentSearchText);
}

void KisFluentBrushPanel::onCategorySelected(QListWidgetItem *item)
{
    if (!item) return;
    m_currentCategory = item->text();
    updateBrushGrid(m_currentCategory, m_currentSearchText);
}

void KisFluentBrushPanel::onBrushThumbnailClicked(const QString& presetName)
{
    selectBrush(presetName);
}

void KisFluentBrushPanel::updateBrushGrid(const QString& category, const QString& filterText)
{
    // Clear grid
    QLayoutItem *child;
    while ((child = m_brushGridLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    // Populate grid (Mock implementation for layout)
    int row = 0;
    int col = 0;
    int maxCols = 3;

    // We would loop through m_allPresets here and filter by category and text.
    // For now, insert a few dummy items
    for (int i = 0; i < 9; ++i) {
        QPushButton *btn = new QPushButton();
        btn->setFixedSize(80, 80);
        btn->setStyleSheet("background: #3A3A3C; border-radius: 12px;");
        // Simulated connection
        connect(btn, &QPushButton::clicked, [this, i]() {
            onBrushThumbnailClicked(QString("Preset_%1").arg(i));
        });
        m_brushGridLayout->addWidget(btn, row, col);
        
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}
