#include "listitemprovider.h"

#include <bb/cascades/Color>
#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/TextFormat>
#include <bb/cascades/TextStyle>
#include <bb/cascades/ListView>
#include <bb/cascades/ScrollAnimation>
#include <bb/cascades/ScrollPosition>

#include "chatlinemodel.h"
#include "datamodeladapter.h"
#include "message.h"

using namespace bb::cascades;

ChatLineProvider::ChatLineProvider()
    : m_scrollToNewLine(false)
{
}

VisualNode * ChatLineProvider::createItem(ListView* list, const QString &type)
{
    Q_UNUSED(type);
    Q_UNUSED(list);

    ChatLine *line = new ChatLine();
    m_scrollToNewLine = true;
    return line;
}

void ChatLineProvider::updateItem(ListView* list, bb::cascades::VisualNode *listItem,
    const QString &type, const QVariantList &indexPath, const QVariant &data)
{
    Q_UNUSED(type);
    Q_UNUSED(data);

    QModelIndex msgIndex = qobject_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, 2);

    int flags = msgIndex.data(MessageModel::FlagsRole).value<int>();

    QModelIndex contentsModelIndex = qobject_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, (int)ChatLineModel::ContentsColumn);
    QModelIndex senderModelIndex = qobject_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, (int)ChatLineModel::SenderColumn);
    ChatLine *line = qobject_cast<ChatLine*>(listItem);
    QString sender = senderModelIndex.data(ChatLineModel::DisplayRole).toString();
    QString contents = contentsModelIndex.data(ChatLineModel::DisplayRole).toString();
    contents.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;");
    sender.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;");
    QString msg = "<html><span style='font-family:Courier New; color:orange'>" + sender + "</span> <span style='font-family:Courier New;'>" + contents + "</span></html>";
    line->updateItem(msg);
    if (flags & Message::Highlight)
        line->setHighlight(true);
    if (m_scrollToNewLine) {
        list->scrollToPosition(ScrollPosition::End, ScrollAnimation::Default);
        m_scrollToNewLine = false;
    }
}

////////////////////////// ChatLine ////////////////////////////////////
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

//////////////////////// ChannelListItemProvider ////////////////////////////
ChannelListItemProvider::ChannelListItemProvider()
{
}

VisualNode * ChannelListItemProvider::createItem(ListView* list, const QString &type)
{
    Q_UNUSED(list);
    if (type == "header")
        return new ChannelListHeader();

    return new ChannelListItem();
}

void ChannelListItemProvider::updateItem(ListView* list, VisualNode *listItem, const QString &type, const QVariantList &indexPath, const QVariant &data)
{
    Q_UNUSED(list);
    Q_UNUSED(indexPath);
    if (type == "header") {
        ChannelListHeader* header = qobject_cast<ChannelListHeader*>(listItem);
        header->updateItem(data.toString());
    } else {
        ChannelListItem* item = qobject_cast<ChannelListItem*>(listItem);
        QModelIndex itemModelIndex = qobject_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, 0);
        QString name = itemModelIndex.data(ChatLineModel::DisplayRole).toString();
        uint foreground = itemModelIndex.data(Qt::ForegroundRole).toUInt();
        //QString text = "<html><span style='font-family:Courier New; font-size:18pt; color:" + foreground + ";'>" + name + "</span></html>";
        //qDebug() << "xxxxx ChannelListItemProvider::updateItem " << text << foreground;
        item->updateItem(name, foreground);
    }
}

/////////////////////// ChannelListHeader //////////////////////////////////
ChannelListHeader::ChannelListHeader(Container* parent)
    : CustomControl(parent)
{
    m_container = new Container();
    m_container->setLayout(new DockLayout());
    m_container->setPreferredHeight(50.0f);
    m_container->setHorizontalAlignment(HorizontalAlignment::Fill);

    Container* bottomBorder = new Container();
    bottomBorder->setPreferredHeight(5.0f);
    bottomBorder->setBackground(Color::DarkBlue);
    bottomBorder->setHorizontalAlignment(HorizontalAlignment::Fill);
    bottomBorder->setVerticalAlignment(VerticalAlignment::Bottom);

    Container* innerContainer = new Container();
    StackLayout *contentLayout = new StackLayout();
    contentLayout->setOrientation(LayoutOrientation::LeftToRight);
    innerContainer->setLayout(contentLayout);
    innerContainer->setVerticalAlignment(VerticalAlignment::Top);
    innerContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    innerContainer->setLeftPadding(3.0f);
    innerContainer->setPreferredHeight(45.0f);

    TextStyle textStyle;
    textStyle.setFontSizeValue(FontSize::Medium);
    textStyle.setFontWeight(FontWeight::Bold);
    textStyle.setColor(Color::LightGray);

    m_itemLabel = Label::create().text("").textStyle(textStyle);
    m_itemLabel->setVerticalAlignment(VerticalAlignment::Center);
    m_itemLabel->setMultiline(false);
    m_itemLabel->setLayoutProperties(StackLayoutProperties::create().spaceQuota(1));

    innerContainer->add(m_itemLabel);
    m_container->add(innerContainer);
    m_container->add(bottomBorder);

    setRoot(m_container);
}
void ChannelListHeader::updateItem(const QString text)
{
    m_itemLabel->setText(text);
}
void ChannelListHeader::select(bool select)
{
    Q_UNUSED(select);
}
void ChannelListHeader::reset(bool selected, bool activated)
{
    Q_UNUSED(selected);
    Q_UNUSED(activated);
}
void ChannelListHeader::activate(bool activate)
{
    Q_UNUSED(activate);
}

/////////////////////// ChannelListItem //////////////////////////////////
ChannelListItem::ChannelListItem(Container* parent)
    : CustomControl(parent)
{
    m_container = new Container();
    DockLayout *contentLayout = new DockLayout();
    m_container->setLayout(contentLayout);
    m_container->setHorizontalAlignment(HorizontalAlignment::Fill);
    m_container->setPreferredHeight(70.0f);

    Container* bottomBorder = new Container();
    bottomBorder->setPreferredHeight(2.0f);
    bottomBorder->setBackground(Color::fromARGB(0xff323232));
    bottomBorder->setHorizontalAlignment(HorizontalAlignment::Fill);
    bottomBorder->setVerticalAlignment(VerticalAlignment::Bottom);

    Container* innerContainer = new Container();
    StackLayout *innerLayout = new StackLayout();
    innerLayout->setOrientation(LayoutOrientation::LeftToRight);
    innerContainer->setLayout(innerLayout);
    innerContainer->setVerticalAlignment(VerticalAlignment::Top);
    innerContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    innerContainer->setLeftPadding(55.0f);
    innerContainer->setPreferredHeight(68.0f);

    TextStyle style = SystemDefaults::TextStyles::titleText();
    m_itemLabel = Label::create().text("").textStyle(style);
    m_itemLabel->setVerticalAlignment(VerticalAlignment::Center);
    m_itemLabel->setHorizontalAlignment(HorizontalAlignment::Fill);
    m_itemLabel->setMultiline(false);
    m_itemLabel->setLayoutProperties(StackLayoutProperties::create().spaceQuota(1));
    innerContainer->add(m_itemLabel);

    m_container->add(bottomBorder);
    m_container->add(innerContainer);

    setRoot(m_container);
}
void ChannelListItem::updateItem(const QString text, const uint color)
{
    m_itemLabel->setText(text);
    m_itemLabel->textStyle()->setColor(Color::fromARGB(color));
}
void ChannelListItem::select(bool select)
{
    Q_UNUSED(select);
}
void ChannelListItem::reset(bool selected, bool activated)
{
    Q_UNUSED(selected);
    Q_UNUSED(activated);
}
void ChannelListItem::activate(bool activate)
{
    Q_UNUSED(activate);
}
