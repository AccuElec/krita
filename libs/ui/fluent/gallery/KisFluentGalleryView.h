// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 KritaFluent Contributors

#ifndef KIS_FLUENT_GALLERY_VIEW_H
#define KIS_FLUENT_GALLERY_VIEW_H

#include <QWidget>
#include <QString>
#include <QDateTime>
#include <QPixmap>
#include <QList>
#include <QScrollArea>

/**
 * @brief Information about an artwork displayed in the gallery.
 */
struct KisFluentArtworkInfo {
    QString filePath;
    QString name;
    QDateTime modified;
    qint64 sizeBytes;
    QPixmap thumbnail;
};

/**
 * @brief The Gallery View serving as the home/launch screen for Fluent Mode.
 */
class KisFluentGalleryView : public QWidget
{
    Q_OBJECT
public:
    explicit KisFluentGalleryView(QWidget *parent = nullptr);
    ~KisFluentGalleryView() override;

    /**
     * @brief Sets the artworks to be displayed in the gallery.
     * @param artworks List of artworks.
     */
    void setArtworks(const QList<KisFluentArtworkInfo> &artworks);

    /**
     * @brief Refreshes the thumbnails for the displayed artworks.
     */
    void refreshThumbnails();

signals:
    /**
     * @brief Emitted when an artwork card is clicked.
     */
    void artworkOpened(const QString &filePath);

    /**
     * @brief Emitted when the "New Canvas" card is clicked.
     */
    void newCanvasRequested();

    /**
     * @brief Emitted when one or more artworks are deleted.
     */
    void artworksDeleted(const QStringList &paths);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif // KIS_FLUENT_GALLERY_VIEW_H
