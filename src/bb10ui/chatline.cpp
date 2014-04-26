#include "chatline.h"

#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/StackLayout>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/TextStyle>

using namespace bb::cascades;

ChatLine::ChatLine(Container *parent) :
    CustomControl(parent)
{
  m_container = new Container();
  StackLayout *contentLayout = new StackLayout();
  contentLayout->setOrientation(LayoutOrientation::LeftToRight);
  m_container->setLeftPadding(3.0f);
  m_container->setLayout(contentLayout);
  m_container->setHorizontalAlignment(HorizontalAlignment::Fill);

  m_itemLabel = Label::create().text("");
  m_itemLabel->setVerticalAlignment(VerticalAlignment::Center);
  m_itemLabel->setMultiline(true);
  m_itemLabel->setLeftMargin(30.0f);

  m_container->add(m_itemLabel);

  setRoot(m_container);
}

void ChatLine::updateItem(const QString text)
{
    m_itemLabel->setText(text);
}

void ChatLine::select(bool select)
{
    Q_UNUSED(select);
}

void ChatLine::reset(bool selected, bool activated)
{
    Q_UNUSED(activated);
    Q_UNUSED(selected);
}

void ChatLine::activate(bool activate)
{
    Q_UNUSED(activate);
}

void ChatLine::setHighlight(bool b)
{
    if (b)
        m_container->setBackground(Color::DarkGray);
    else
        m_container->resetBackground();
}
