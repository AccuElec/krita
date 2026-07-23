// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KIS_FLUENT_ANIMATION_TIMELINE_H
#define KIS_FLUENT_ANIMATION_TIMELINE_H

#include <QWidget>
#include <QScrollArea>
#include <QPropertyAnimation>

class QPainter;
class QMenu;
class QPushButton;
class QSlider;
class QComboBox;

/**
 * @brief The KisFluentAnimationTimeline class
 *
 * A collapsible panel at the bottom of the canvas providing a horizontal strip
 * of frame thumbnails, playback controls, and onion skin toggles. It supports
 * touch gestures for scrubbing, zooming, and frame options.
 */
class KisFluentAnimationTimeline : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentFrame READ currentFrame WRITE setCurrentFrame NOTIFY currentFrameChanged)
    Q_PROPERTY(int frameCount READ frameCount WRITE setFrameCount NOTIFY frameCountChanged)
    Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY fpsChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(bool onionSkinEnabled READ onionSkinEnabled WRITE setOnionSkinEnabled NOTIFY onionSkinEnabledChanged)

public:
    explicit KisFluentAnimationTimeline(QWidget *parent = nullptr);
    ~KisFluentAnimationTimeline() override;

    int currentFrame() const;
    void setCurrentFrame(int frame);

    int frameCount() const;
    void setFrameCount(int count);

    int fps() const;
    void setFps(int fps);

    bool isPlaying() const;
    void setIsPlaying(bool playing);

    bool onionSkinEnabled() const;
    void setOnionSkinEnabled(bool enabled);

    void showPanel();
    void hidePanel();

signals:
    void frameSelected(int frame);
    void playbackToggled(bool playing);
    void fpsChanged(int fps);
    void frameAdded(int afterFrame);
    void frameDeleted(int frame);
    void panelToggled(bool visible);

    void currentFrameChanged();
    void frameCountChanged();
    void isPlayingChanged();
    void onionSkinEnabledChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void togglePlayback();
    void toggleOnionSkin();
    void addFrame();
    void showFrameOptions(int frame, const QPoint &pos);

private:
    void setupUi();
    void updateThumbnails();

    int m_currentFrame {0};
    int m_frameCount {1};
    int m_fps {24};
    bool m_isPlaying {false};
    bool m_onionSkinEnabled {false};
    bool m_isVisible {false};

    float m_zoomFactor {1.0f};

    QScrollArea *m_scrollArea {nullptr};
    QWidget *m_framesContainer {nullptr};
    QPushButton *m_playPauseButton {nullptr};
    QPushButton *m_loopButton {nullptr};
    QComboBox *m_fpsSelector {nullptr};
    QPushButton *m_onionSkinButton {nullptr};
    QSlider *m_onionSkinOpacitySlider {nullptr};
    QPushButton *m_addFrameButton {nullptr};

    QPropertyAnimation *m_slideAnimation {nullptr};
};

#endif // KIS_FLUENT_ANIMATION_TIMELINE_H
