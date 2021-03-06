#include "powerwidget.h"
#include <QHBoxLayout>
#include <QIcon>
#include <QPixmap>
#include <QPainter>

namespace Plugins {
    namespace Power {
        PowerWidget::PowerWidget(QWidget *parent) : QWidget(parent) {
            m_popup = new PowerPopupWidget;

            setFixedHeight(25);

            m_batteryIcon = new QLabel;
            m_battery = new QLabel;

            m_battery->setAlignment(Qt::AlignVCenter);
            m_batteryIcon->setAlignment(Qt::AlignVCenter);

            QHBoxLayout *layout = new QHBoxLayout;
            layout->setMargin(0);
            layout->setSpacing(5);
            layout->addWidget(m_batteryIcon, 0, Qt::AlignVCenter);
            layout->addWidget(m_battery, 0, Qt::AlignVCenter);

            setLayout(layout);

            m_battery->setStyleSheet("QLabel {"
                                     "font: 16px;"
                                     "color: black;"
                                     "}");


            m_powerInter = new DBusPower(this);

            updateBatteryIcon();

            connect(m_powerInter, &DBusPower::BatteryPercentageChanged, this, &PowerWidget::updateBatteryIcon);
            connect(m_powerInter, &DBusPower::BatteryStateChanged, this, &PowerWidget::updateBatteryIcon);
            connect(m_powerInter, &DBusPower::OnBatteryChanged, this, &PowerWidget::updateBatteryIcon);
            connect(m_popup, &PowerPopupWidget::requestPowerPrecent, m_battery, &QLabel::setVisible);
            connect(m_popup, &PowerPopupWidget::requestHidePopup, this, &PowerWidget::requestHidePopup);
        }

        QWidget *PowerWidget::popup() {
            return m_popup;
        }

        void PowerWidget::updateBatteryIcon() {
            const BatteryPercentageMap data = m_powerInter->batteryPercentage();

            if (data.isEmpty()) {
                m_battery->setText(tr("UPS"));
                m_batteryIcon->setPixmap(QPixmap(":/Icons/battery-100.png"));
                return;
            }

            const uint value = qMin(100.0, qMax(0.0, data.value("Display")));
            const int percentage = std::round(value);
            const bool plugged = !m_powerInter->onBattery();

            QString percentageStr;
            if (percentage < 10 && percentage >= 0) {
                percentageStr = "000";
            } else if (percentage < 30) {
                percentageStr = "020";
            } else if (percentage < 50) {
                percentageStr = "040";
            } else if (percentage < 70) {
                percentageStr = "060";
            } else if (percentage < 90) {
                percentageStr = "080";
            } else if (percentage <= 100){
                percentageStr = "100";
            } else {
                percentageStr = "000";
            }

            QString iconStr;

            if (plugged) {
                iconStr = QString("battery-%1-charging.png").arg(percentageStr);
            } else {
                iconStr = QString("battery-%1.png").arg(percentageStr);
            }
            m_battery->setText(percentageStr + "%");
            m_batteryIcon->setPixmap(QPixmap(":/Icons/" + iconStr));
        }

        void PowerWidget::enterEvent(QEvent *event)
        {
            QWidget::enterEvent(event);

            m_enter = true;

            m_battery->setStyleSheet("QLabel {"
                                     "font: 16px;"
                                     "color: white;"
                                     "}");

            update();
        }

        void PowerWidget::leaveEvent(QEvent *event)
        {
            QWidget::leaveEvent(event);

            m_enter = false;

            m_battery->setStyleSheet("QLabel {"
                                     "font: 16px;"
                                     "color: black;"
                                     "}");

            update();
        }

        void PowerWidget::paintEvent(QPaintEvent *event)
        {
            QWidget::paintEvent(event);

            QPainter painter(this);

            if (m_enter) {
                painter.fillRect(rect(), QColor("#1E90FF"));
            }
        }
    }
}
