#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QFrame>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QPropertyAnimation>
//dbus
#include <com_deepin_dde_launcher.h>

using LauncherInter = com::deepin::dde::Launcher;

#define TOPHEIGHT 25

class MainPanel;

class MainFrame : public QFrame
{
    Q_OBJECT

public:
    explicit MainFrame(QWidget *parent = 0);
    ~MainFrame();

    void registerDesktop();

    void init();
    void initConnect();
    void initAnimation();

private slots:
    void screenChanged();

private:
    QDesktopWidget *m_desktopWidget;
    MainPanel *m_mainPanel;
    LauncherInter *m_launcherInter;
    QPropertyAnimation *m_hideWithLauncher;
    QPropertyAnimation *m_showWithLauncher;
};

#endif // MAINFRAME_H
