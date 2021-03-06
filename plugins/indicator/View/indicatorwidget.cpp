#include "indicatorwidget.h"
#include <QHBoxLayout>
#include <QIcon>
#include <QMouseEvent>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include "themeappicon.h"

namespace Plugin {
namespace Indicator {

IndicatorWidget::IndicatorWidget(QWidget *parent) : QWidget(parent)
{
    initUI();

    m_dockInter = new DBusDock(this);
    m_smallWatcher= new QFutureWatcher<QPixmap>(this);

    connect(m_dockInter, &DBusDock::EntryAdded, this, &IndicatorWidget::addEntry);
    connect(m_dockInter, &DBusDock::EntryRemoved, this, &IndicatorWidget::removeEntry);

    connect(m_smallWatcher, &QFutureWatcher<QPixmap>::finished, this, &IndicatorWidget::refreshIcon);

    getAllEntry();
}

IndicatorWidget::~IndicatorWidget()
{
    m_dockInter->deleteLater();
}

void IndicatorWidget::initUI()
{
    setWindowFlags(Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);

    m_entry = new Entry;
    m_entry->resize(QSize(23, 23));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_entry, 0, Qt::AlignLeft);

    setLayout(mainLayout);
}

void IndicatorWidget::getAllEntry()
{
    for (const QDBusObjectPath &entryPath : m_dockInter->entries())
        addEntry(entryPath, m_entryList.count());
}

void IndicatorWidget::addEntry(const QDBusObjectPath &entryPath, const int index)
{
    Q_UNUSED(index);

    DBusDockEntry *entry = new DBusDockEntry(entryPath.path());
    m_entryList.append(entry);

    connect(entry, &DBusDockEntry::ActiveChanged, this, &IndicatorWidget::refreshActiveWindow, Qt::UniqueConnection);
    connect(entry, &DBusDockEntry::TitlesChanged, this, [=] {
        m_entry->setText(windowTitle(entry->titles()));
    });

    refreshActiveWindow();
}

void IndicatorWidget::removeEntry(const QString &entryID)
{
    for (int i = 0; i != m_entryList.count(); i++) {
        if (m_entryList.at(i)->id() == entryID) {
            DBusDockEntry *entry = m_entryList.at(i);
            m_entryList.removeAt(i);
            entry->disconnect();
            entry->deleteLater();
            break;
        }
    }

    refreshActiveWindow();
}

void IndicatorWidget::refreshActiveWindow()
{
    for (DBusDockEntry *entry : m_entryList) {
        if (entry->active()) {
            m_smallWatcher->cancel();
            m_smallWatcher->setFuture(QtConcurrent::run(ThemeAppIcon::getIcon, entry->icon()));
            m_entry->setText(windowTitle(entry->titles()));
            m_entry->setVisible(true);
            return;
        }
    }

    m_entry->setVisible(false);
}

void IndicatorWidget::refreshIcon()
{
    m_entry->setNormalIcon(QIcon(m_smallWatcher->result()));
}

const QString IndicatorWidget::windowTitle(const WindowDict &infos)
{
    for (auto it(infos.cbegin()); it != infos.cend(); ++it)
    {
        return it.value();
    }

    return QString();
}

}
}
