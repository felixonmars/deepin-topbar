#include "mainframe.h"
#include "mainpanel.h"
#include <X11/X.h>
#include <X11/Xutil.h>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <QtX11Extras/QX11Info>
#include <QScreen>
#include <QApplication>
#include <QRect>
#include <DForeignWindow>

DWIDGET_USE_NAMESPACE

#define DEFINE_CONST_CHAR(Name) const char _##Name[] = "_d_" #Name

// functions
DEFINE_CONST_CHAR(getWindows);

MainFrame::MainFrame(QWidget *parent): QFrame(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedHeight(TOPHEIGHT);

    init();
    initAnimation();
    screenChanged();
    initConnect();
}

MainFrame::~MainFrame()
{
    m_desktopWidget->deleteLater();
}

void MainFrame::registerDesktop()
{
    QFunctionPointer wmClientList = Q_NULLPTR;
    QList<DForeignWindow*> windowList;

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    wmClientList = qApp->platformFunction(_getWindows);
#endif

    if (wmClientList) {
        for (WId wid : reinterpret_cast<QVector<quint32>(*)()>(wmClientList)()) {
            if (DForeignWindow *w = DForeignWindow::fromWinId(wid)) {
                windowList << w;
            }
        }
    }

    for (DForeignWindow *window : windowList) {
       if (window->wmClass() == "dde-desktop") {
           windowHandle()->setParent(window);
           break;
       }
    }
}

void MainFrame::init()
{
    m_desktopWidget = QApplication::desktop();

    m_mainPanel = new MainPanel(this);

    // I can't handle it properly. unable keep out launcher

//    m_launcherInter = new LauncherInter("com.deepin.dde.Launcher",
//                                        "/com/deepin/dde/Launcher",
//                                        QDBusConnection::sessionBus(),
//                                        this);
}

void MainFrame::initConnect()
{
    connect(m_desktopWidget, &QDesktopWidget::resized, this, &MainFrame::screenChanged);

//    connect(m_launcherInter, &LauncherInter::Shown, this, [=]{
//        m_hideWithLauncher->start();
//    });

//    connect(m_launcherInter, &LauncherInter::Closed, this, [=]{
//        m_showWithLauncher->start();
//    });

//    connect(m_showWithLauncher, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
//        m_blurEffectWidget->move(value.toPoint());
//    });

//    connect(m_hideWithLauncher, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
//        m_blurEffectWidget->move(value.toPoint());
//    });

//    connect(m_showWithLauncher, &QPropertyAnimation::finished, this, [=]{
//    });

//    connect(m_hideWithLauncher, &QPropertyAnimation::finished, this, [=]{

//    });
}

void MainFrame::initAnimation()
{
//    m_showWithLauncher =new QPropertyAnimation(m_mainPanel, "pos", m_mainPanel);
//    m_showWithLauncher->setDuration(300);
//    m_showWithLauncher->setStartValue(QPoint(m_mainPanel->x(), -m_mainPanel->height()));
//    m_showWithLauncher->setEndValue(QPoint(m_mainPanel->x(), 0));
//    m_showWithLauncher->setEasingCurve(QEasingCurve::InOutCubic);

//    m_hideWithLauncher =new QPropertyAnimation(m_mainPanel, "pos", m_mainPanel);
//    m_hideWithLauncher->setDuration(300);
//    m_hideWithLauncher->setStartValue(QPoint(m_mainPanel->x(), 0));
//    m_hideWithLauncher->setEndValue(QPoint(m_mainPanel->x(), -m_mainPanel->height()));
    //    m_hideWithLauncher->setEasingCurve(QEasingCurve::InOutCubic);
}

void MainFrame::screenChanged()
{
    QRect screen = m_desktopWidget->screenGeometry(m_desktopWidget->primaryScreen());
    resize(screen.width(), TOPHEIGHT);
    m_mainPanel->resize(screen.width(), TOPHEIGHT);
    move(screen.x(), 0);
    m_mainPanel->move(0, 0);

    xcb_ewmh_connection_t m_ewmh_connection;
    xcb_intern_atom_cookie_t *cookie = xcb_ewmh_init_atoms(QX11Info::connection(), &m_ewmh_connection);
    xcb_ewmh_init_atoms_replies(&m_ewmh_connection, cookie, NULL);

    xcb_atom_t atoms[1];
    atoms[0] = m_ewmh_connection._NET_WM_WINDOW_TYPE_DESKTOP;
    xcb_ewmh_set_wm_window_type(&m_ewmh_connection, winId(), 1, atoms);
}
