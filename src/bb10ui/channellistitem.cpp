#include "channellistitem.h"

#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Label>
#include <bb/cascades/StackLayout>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/TextStyle>

ChannelListItem::ChannelListItem(Container *parent) : CustomControl(parent)
{
    // A background Container that will hold a background image and an item content Container
    Container *itemContainer = new Container();
    DockLayout *itemLayout = new DockLayout();
    itemContainer->setLayout(itemLayout);
    itemContainer->setPreferredWidth(768.0f);

    // The white background image for an item
    ImageView *bkgImage = ImageView::create("asset:///images/white_photo.png").preferredSize(768.0f, 173.0f);

    // A Colored Container will be used to show if an item is highlighted
    m_highlighContainer = new Container();
    m_highlighContainer->setBackground(Color::fromARGB(0xff75b5d3));
    m_highlighContainer->setHorizontalAlignment(HorizontalAlignment::Center);
    m_highlighContainer->setOpacity(0.0);
    m_highlighContainer->setPreferredWidth(760.0f);
    m_highlighContainer->setPreferredHeight(168.0f);

    // Content Container containing an image and label with padding for the alignment
    // on background image. Note that we disable implicit layout animations to avoid
    // unsynchronized animations on the list items as the item image is asynchronously loaded.
    Container *contentContainer = new Container();
    StackLayout *contentLayout = new StackLayout();
    contentLayout->setOrientation(LayoutOrientation::LeftToRight);
    contentContainer->setLeftPadding(3.0f);
    contentContainer->setLayout(contentLayout);
    contentContainer->setImplicitLayoutAnimationsEnabled(false);

    // A list item label, docked to the center, the text is set in updateItem.
    m_itemLabel = Label::create().text(" ");
    m_itemLabel->setVerticalAlignment(VerticalAlignment::Center);
    m_itemLabel->setLeftMargin(30.0f);
    m_itemLabel->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    m_itemLabel->textStyle()->setColor(Color::Black);
    m_itemLabel->setImplicitLayoutAnimationsEnabled(false);

    contentContainer->add(m_itemLabel);

    // Add the background image and the content to the full item container.
    itemContainer->add(bkgImage);
    itemContainer->add(m_highlighContainer);
    itemContainer->add(contentContainer);

    setRoot(itemContainer);
}

void ChannelListItem::updateItem(const QString text)
{
    m_itemLabel->setText(text);
}

void ChannelListItem::select(bool select)
{
    // When an item is selected, show the colored highlight Container
    if (select)
        m_highlighContainer->setOpacity(0.9f);
    else
        m_highlighContainer->setOpacity(0.0f);
}

void ChannelListItem::reset(bool selected, bool activated)
{
    Q_UNUSED(activated);

    // Since items are recycled, the reset function is where we have
    // to make sure that item state (defined by the arguments) is correct.
    select(selected);
}

void ChannelListItem::activate(bool activate)
{
    // There is no special activate state; selected and activated look the same.
    select(activate);
}

