#include "nicklistitem.h"

#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/StackLayout>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/TextStyle>

using namespace bb::cascades;

NickListItem::NickListItem(Container *parent) :
    CustomControl(parent)
{
  // A background Container that will hold a background image and an item content Container
  Container *itemContainer = new Container();
  DockLayout *itemLayout = new DockLayout();
  itemContainer->setLayout(itemLayout);
  //itemContainer->setPreferredWidth(768.0f);

  // A Colored Container will be used to show if an item is highlighted
  mHighlighContainer = new Container();
  //mHighlighContainer->setBackground(Color::fromARGB(0xff75b5d3));
  mHighlighContainer->setHorizontalAlignment(HorizontalAlignment::Center);
  mHighlighContainer->setOpacity(0.0);
  //mHighlighContainer->setPreferredWidth(760.0f);
  //mHighlighContainer->setPreferredHeight(168.0f);

  // Content Container containing an image and label with padding for the alignment on background image
  Container *contentContainer = new Container();
  StackLayout *contentLayout = new StackLayout();
  contentLayout->setOrientation(LayoutOrientation::LeftToRight);
  contentContainer->setLeftPadding(3.0f);
  contentContainer->setLayout(contentLayout);

  // A list item label, docked to the center, the text is set in updateItem.
  mItemLabel = Label::create().text(" ");
  mItemLabel->setVerticalAlignment(VerticalAlignment::Center);
  mItemLabel->setMultiline(true);
  mItemLabel->setLeftMargin(30.0f);
  //mItemLabel->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
  //mItemLabel->textStyle()->setColor(Color::Black);

  // Add the Image and Label to the content Container
  contentContainer->add(mItemLabel);

  // Add the background image and the content to the full item container.
  itemContainer->add(mHighlighContainer);
  itemContainer->add(contentContainer);

  setRoot(itemContainer);
}

void NickListItem::updateItem(const QString text)
{
    mItemLabel->setText(text);
}

void NickListItem::select(bool select)
{
  // When an item is selected, show the colored highlight Container
  if (select) {
    mHighlighContainer->setOpacity(0.9f);
  } else {
    mHighlighContainer->setOpacity(0.0f);
  }
}

void NickListItem::reset(bool selected, bool activated)
{
  Q_UNUSED(activated);

  // Since items are recycled, the reset function is where we have
  // to make sure that item state (defined by the arguments) is correct.
  select(selected);
}

void NickListItem::activate(bool activate)
{
  // There is no special activate state; selected and activated look the same.
  select(activate);
}

/////////////////// NickListHeader /////////////////////////

NickListHeader::NickListHeader(Container *parent) :
    CustomControl(parent)
{
  // A background Container that will hold a background image and an item content Container
  Container *itemContainer = new Container();
  DockLayout *itemLayout = new DockLayout();
  itemContainer->setLayout(itemLayout);

  // A Colored Container will be used to show if an item is highlighted
  mHighlighContainer = new Container();
  mHighlighContainer->setBackground(Color::fromARGB(0xff808080));
  mHighlighContainer->setHorizontalAlignment(HorizontalAlignment::Center);
  mHighlighContainer->setOpacity(0.5);
  //mHighlighContainer->setPreferredWidth(760.0f);
  //mHighlighContainer->setPreferredHeight(168.0f);

  // Content Container containing an image and label with padding for the alignment on background image
  Container *contentContainer = new Container();
  StackLayout *contentLayout = new StackLayout();
  contentLayout->setOrientation(LayoutOrientation::LeftToRight);
  contentContainer->setLeftPadding(3.0f);
  contentContainer->setLayout(contentLayout);

  // A list item label, docked to the center, the text is set in updateItem.
  mItemLabel = Label::create().text(" ");
  mItemLabel->setVerticalAlignment(VerticalAlignment::Center);
  mItemLabel->setMultiline(true);
  mItemLabel->setLeftMargin(30.0f);
  mItemLabel->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
  //mItemLabel->textStyle()->setColor(Color::Black);

  // Add the Image and Label to the content Container
  contentContainer->add(mItemLabel);

  // Add the background image and the content to the full item container.
  itemContainer->add(mHighlighContainer);
  itemContainer->add(contentContainer);

  setRoot(itemContainer);
}

void NickListHeader::updateItem(const QString text)
{
    mItemLabel->setText(text);
}

void NickListHeader::select(bool select)
{
    Q_UNUSED(select);
}

void NickListHeader::reset(bool selected, bool activated)
{
  Q_UNUSED(activated);

  // Since items are recycled, the reset function is where we have
  // to make sure that item state (defined by the arguments) is correct.
  select(selected);
}

void NickListHeader::activate(bool activate)
{
  // There is no special activate state; selected and activated look the same.
  select(activate);
}
