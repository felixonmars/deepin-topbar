#include "notifywidget.h"
#include <QIcon>
#include <QHBoxLayout>
#include <QLabel>

NotifyWidget::NotifyWidget(QWidget *parent) : QLabel(parent)
{
      setFixedSize(25, 25);

      setStyleSheet("QLabel {"
                    "color: black;"
                    "background: transparent;"
                    "}"
                    "QLabel:hover {"
                    "color: white;"
                    "background: #1E90FF;"
                    "}");

      QLabel *label = new QLabel;
      label->setFixedSize(22, 22);
      label->setPixmap(QIcon::fromTheme("view-list-details").pixmap(22, 22));

      QHBoxLayout *layout = new QHBoxLayout;

      layout->setMargin(0);
      layout->setSpacing(0);
      layout->addWidget(label, 0, Qt::AlignCenter);

      setLayout(layout);
}
