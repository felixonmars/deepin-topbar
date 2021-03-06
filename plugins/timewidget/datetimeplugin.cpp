#include "datetimeplugin.h"
#include "datetimepopup.h"

DateTimePlugin::DateTimePlugin(QWidget *parent)
    : QObject(parent) {
    m_centralWidget = new Plugin::DateTime::DateTimeWidget;

    connect(m_centralWidget->popupWidget(), &Plugin::DateTime::DateTimePopup::requestHide, this, [=] {
        m_proxyInter->requestHidePopup();
    });
}

DateTimePlugin::~DateTimePlugin() {

}

const QString DateTimePlugin::pluginName() const {
    return QString("datetime");
}

void DateTimePlugin::init(PluginProxyInterface *proxyInter) {
    m_proxyInter = proxyInter;

    m_proxyInter->itemAdded(this, QString());
}

int DateTimePlugin::itemSortKey(const QString &itemKey) {
    Q_UNUSED(itemKey);

    return 0;
}

QWidget *DateTimePlugin::itemWidget(const QString &itemKey) {
    Q_UNUSED(itemKey);

    return m_centralWidget;
}

QWidget *DateTimePlugin::itemPopupApplet(const QString &itemKey) {
    Q_UNUSED(itemKey);

    return m_centralWidget->popupWidget();
}

const QString DateTimePlugin::itemCommand(const QString &itemKey) {
    Q_UNUSED(itemKey);

    return QString("dde-datetime");
}

void DateTimePlugin::popupShow()
{
}

void DateTimePlugin::popupHide()
{
    m_proxyInter->requestHidePopup();
}
