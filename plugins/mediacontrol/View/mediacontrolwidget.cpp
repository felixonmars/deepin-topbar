#include "mediacontrolwidget.h"
#include <QHBoxLayout>
#include <QDBusInterface>

namespace Plugin {
    namespace MediaControl {
        MediaControlWidget::MediaControlWidget(QWidget *parent) : QFrame(parent) {
            setWindowFlags(Qt::FramelessWindowHint);
            setAttribute(Qt::WA_TranslucentBackground);

            setFixedSize(100, 25);

            setVisible(false);

            m_mediaTitle = new TextTicker(this);
            m_mediaControl = new MediaControl(this);

            m_mediaTitle->resize(100, 25);
            m_mediaControl->resize(100, 25);
            m_mediaControl->move(0, -m_mediaControl->height());

            m_mediaTitle->setAlignment(Qt::AlignVCenter);
            m_mediaTitle->setStyleSheet("font-size: 16px;"
                                        "color: black;");

            //Animation
            m_hoverControlAni = new QPropertyAnimation(m_mediaControl, "pos", this);
            m_hoverControlAni->setDuration(300);
            m_hoverControlAni->setStartValue(QPoint(m_mediaControl->x(), 0));
            m_hoverControlAni->setEndValue(QPoint(m_mediaControl->x(), -m_mediaControl->height()));
            m_hoverControlAni->setEasingCurve(QEasingCurve::InOutCubic);

            m_showControlAni = new QPropertyAnimation(m_mediaControl, "pos", this);
            m_showControlAni->setDuration(300);
            m_showControlAni->setStartValue(QPoint(m_mediaControl->x(), -m_mediaControl->height()));
            m_showControlAni->setEndValue(QPoint(m_mediaControl->x(), 0));
            m_showControlAni->setEasingCurve(QEasingCurve::InOutCubic);

            connect(m_hoverControlAni, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
                m_mediaTitle->move(0, 25 + value.toPoint().y());
            });

            connect(m_showControlAni, &QPropertyAnimation::valueChanged ,this, [=](const QVariant &value) {
                m_mediaTitle->move(0, 25 + value.toPoint().y());
            });

            m_mprisInter = nullptr;

            initMpris();
        }

        void MediaControlWidget::initMpris() {
            m_dbusInter = new DBusInterface("org.freedesktop.DBus", "/org/freedesktop/DBus", QDBusConnection::sessionBus(), this);

            connect(m_dbusInter, &DBusInterface::NameOwnerChanged, this, &MediaControlWidget::onNameOwnerChanged);

            for (const auto &name : m_dbusInter->ListNames().value())
                onNameOwnerChanged(name, QString(), name);
        }

        void MediaControlWidget::onNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner) {
            Q_UNUSED(oldOwner);

            if (!name.startsWith("org.mpris.MediaPlayer2."))
                return;

            if (newOwner.isEmpty())
                removeMPRISPath(name);
            else
                loadMPRISPath(name);
        }

        void MediaControlWidget::loadMPRISPath(const QString &path) {
            m_lastPath = path;

            // save paths
            if (!m_mprisPaths.contains(path))
                m_mprisPaths.append(path);

            if (m_mprisInter)
                m_mprisInter->deleteLater();

            m_mprisInter = new DBusMPRIS(path, "/org/mpris/MediaPlayer2", QDBusConnection::sessionBus(), this);

            connect(m_mediaControl, &MediaControl::requestLast, m_mprisInter, &DBusMediaPlayer2::Next, Qt::UniqueConnection);
            connect(m_mediaControl, &MediaControl::requestPrevious, m_mprisInter, &DBusMediaPlayer2::Previous, Qt::UniqueConnection);
            connect(m_mediaControl, &MediaControl::requestPause, m_mprisInter, &DBusMediaPlayer2::PlayPause, Qt::UniqueConnection);

            connect(m_mprisInter, &DBusMediaPlayer2::MetadataChanged, this, [=]{
                m_mediaTitle->setText(m_mprisInter->metadata().value("xesam:title").toString());
            });

            connect(m_mprisInter, &DBusMediaPlayer2::PlaybackStatusChanged, this, [=]{
                if (m_mprisInter->playbackStatus() == "Playing") {
                    m_mediaControl->setPlayState(MediaControl::Play);
                    setVisible(true);
                }
                if (m_mprisInter->playbackStatus() == "Stopped") {
                    m_mediaControl->setPlayState(MediaControl::Stop);
                    setVisible(false);
                }
                if (m_mprisInter->playbackStatus() == "Paused")
                    m_mediaControl->setPlayState(MediaControl::Pause);

                m_mprisInter->MetadataChanged();
            });

            m_mprisInter->MetadataChanged();
            m_mprisInter->PlaybackStatusChanged();
        }

        void MediaControlWidget::removeMPRISPath(const QString &path) {
            m_mprisPaths.removeOne(path);

            if (m_lastPath != path)
                return;

            if (!m_mprisInter)
                return;

            if (!m_mprisPaths.isEmpty())
                return loadMPRISPath(m_mprisPaths.last());

            m_mprisInter->deleteLater();
            m_mprisInter = nullptr;

            setVisible(false);
        }

        void MediaControlWidget::enterEvent(QEvent *event) {
            QFrame::enterEvent(event);

            m_showControlAni->start();
        }

        void MediaControlWidget::leaveEvent(QEvent *event) {
             QFrame::leaveEvent(event);

             m_hoverControlAni->start();
        }
    }
}
