// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#include "KisFluentGalleryView.h"

#include <QPainter>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QApplication>

struct KisFluentGalleryView::Private {
    QList<KisFluentArtworkInfo> artworks;
    QScrollArea *scrollArea;
    QWidget *contentWidget;
    bool selectionMode = false;
    QSet<QString> selectedPaths;
    
    // UI elements
    QLineEdit *searchBar;
    QComboBox *sortDropdown;
    QWidget *bulkActionBar;

    // Layout
    int columns = 3;
    const int cardWidth = 200;
    const int cardHeight = 250;
    const int spacing = 20;
    
    // Drag/Drop/Selection
    QPoint lastPos;
    bool dragging = false;
};

KisFluentGalleryView::KisFluentGalleryView(QWidget *parent)
    : QWidget(parent), d(new Private())
{
    // Full-screen dark background (#1C1C1E)
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor("#1C1C1E"));
    setPalette(pal);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Top Bar: Search and Sort
    QHBoxLayout *topBarLayout = new QHBoxLayout();
    d->searchBar = new QLineEdit(this);
    d->searchBar->setPlaceholderText(tr("Search artworks..."));
    d->searchBar->setStyleSheet("QLineEdit { background: #2C2C2E; color: white; border-radius: 10px; padding: 5px; }");
    
    d->sortDropdown = new QComboBox(this);
    d->sortDropdown->addItems({tr("Date Modified"), tr("Date Created"), tr("Name"), tr("Size")});
    d->sortDropdown->setStyleSheet("QComboBox { background: #2C2C2E; color: white; border-radius: 10px; padding: 5px; }");

    topBarLayout->addWidget(d->searchBar);
    topBarLayout->addStretch();
    topBarLayout->addWidget(d->sortDropdown);
    mainLayout->addLayout(topBarLayout);

    // Scroll Area for Grid
    d->scrollArea = new QScrollArea(this);
    d->scrollArea->setWidgetResizable(true);
    d->scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    
    d->contentWidget = new QWidget(d->scrollArea);
    d->contentWidget->setStyleSheet("background: transparent;");
    d->scrollArea->setWidget(d->contentWidget);
    mainLayout->addWidget(d->scrollArea);

    // Bulk Action Bar (Hidden by default)
    d->bulkActionBar = new QWidget(this);
    QHBoxLayout *bulkLayout = new QHBoxLayout(d->bulkActionBar);
    d->bulkActionBar->setStyleSheet("QWidget { background: #2C2C2E; border-radius: 15px; }");
    QPushButton *btnDelete = new QPushButton(tr("Delete"), d->bulkActionBar);
    QPushButton *btnDuplicate = new QPushButton(tr("Duplicate"), d->bulkActionBar);
    QPushButton *btnExport = new QPushButton(tr("Export"), d->bulkActionBar);
    QPushButton *btnMove = new QPushButton(tr("Move to Stack"), d->bulkActionBar);
    bulkLayout->addWidget(btnDelete);
    bulkLayout->addWidget(btnDuplicate);
    bulkLayout->addWidget(btnExport);
    bulkLayout->addWidget(btnMove);
    d->bulkActionBar->hide();
    mainLayout->addWidget(d->bulkActionBar);
}

KisFluentGalleryView::~KisFluentGalleryView()
{
}

void KisFluentGalleryView::setArtworks(const QList<KisFluentArtworkInfo> &artworks)
{
    d->artworks = artworks;
    update();
}

void KisFluentGalleryView::refreshThumbnails()
{
    update();
}

void KisFluentGalleryView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // Responsive grid: 3 in landscape, 2 in portrait
    d->columns = (width() > height()) ? 3 : 2;
    // Real implementation would relayout d->contentWidget here
}

void KisFluentGalleryView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    // Custom painting of cards would be implemented here, on d->contentWidget.
    // Since we are requested to just create the structure, we can leave this stub.
}

void KisFluentGalleryView::mousePressEvent(QMouseEvent *event)
{
    d->lastPos = event->pos();
    // Check if new canvas card or artwork clicked
    // Logic for long press activation for selection mode would go here
    QWidget::mousePressEvent(event);
}

void KisFluentGalleryView::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->pos() - d->lastPos).manhattanLength() > QApplication::startDragDistance()) {
        d->dragging = true;
    }
    QWidget::mouseMoveEvent(event);
}

void KisFluentGalleryView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!d->dragging) {
        // Handle click
        // emit newCanvasRequested() or emit artworkOpened()
    }
    d->dragging = false;
    QWidget::mouseReleaseEvent(event);
}
