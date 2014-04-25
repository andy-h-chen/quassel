/* Copyright (c) 2012 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
  // A background Container that will hold a background image and an item content Container
  Container *itemContainer = new Container();
  DockLayout *itemLayout = new DockLayout();
  itemContainer->setLayout(itemLayout);
  //itemContainer->setPreferredWidth(768.0f);

  // A Colored Container will be used to show if an item is highlighted
  mHighlighContainer = new Container();
  mHighlighContainer->setBackground(Color::fromARGB(0xff808080));
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

void ChatLine::updateItem(const QString text)
{
    mItemLabel->setText(text);
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
  if (activate) {
    mHighlighContainer->setOpacity(0.9f);
  } else {
    mHighlighContainer->setOpacity(0.0f);
  }
}
