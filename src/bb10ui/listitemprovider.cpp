#include "listitemprovider.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/ActionSet>
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

#include <QTextCharFormat>

#include "bb10ui.h"
#include "bb10uistyle.h"
#include "chatlinemodel.h"
#include "datamodeladapter.h"
#include "message.h"
#include "networkmodel.h"

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

    QModelIndex contentsModelIndex = qobject_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, (int)ChatLineModel::ContentsColumn);
    QModelIndex senderModelIndex = qobject_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, (int)ChatLineModel::SenderColumn);
    QModelIndex timeModelIndex = qobject_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, (int)ChatLineModel::TimestampColumn);
    ChatLine *line = qobject_cast<ChatLine*>(listItem);
    QString sender = senderModelIndex.data(ChatLineModel::DisplayRole).toString();
    Bb10UiStyle::MessageLabel label = (Bb10UiStyle::MessageLabel)senderModelIndex.data(ChatLineModel::MsgLabelRole).toInt();
    QTextCharFormat fmt = Bb10Ui::uiStyle()->format(senderModelIndex.data(ChatLineModel::FormatRole).value<Bb10UiStyle::FormatList>().at(0).second, label);
    QString senderForeground = fmt.foreground().color().name();
    QString senderBackground = fmt.background().color().name();

    QString timestamp = timeModelIndex.data(ChatLineModel::DisplayRole).toString();
    QTextCharFormat timeFmt = Bb10Ui::uiStyle()->format(timeModelIndex.data(ChatLineModel::FormatRole).value<Bb10UiStyle::FormatList>().at(0).second, 0);
    QString timeForeground = timeFmt.foreground().color().name();
    QString timeBackground = timeFmt.background().color().name();
    //qDebug() << "xxxxx timestamp =" << timestamp << timeForeground << timeBackground;
    QString contents = contentsModelIndex.data(ChatLineModel::FormatedHtmlContentRole).toString();
    QString contentsBackground = contentsModelIndex.data(ChatLineModel::BackgroundRole).value<QBrush>().color().name();
    //contents.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;");
    sender.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;");
    QString msg = "<html><span style='font-family:Courier New; color:" + timeForeground + "; background-color:" + timeBackground + ";'>" + timestamp + "</span><span style='font-family:Courier New; color:" + senderForeground + "; background-color:" + senderBackground + ";'>" + sender + "</span> <span style='font-family:Courier New; background-color:" + contentsBackground + "'>" + contents + "</span></html>";
    line->updateItem(msg);
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
    m_itemLabel->setLayoutProperties(StackLayoutProperties::create().spaceQuota(1));
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
    Q_UNUSED(data);

    QModelIndex itemModelIndex = qobject_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, 0);
    QString name = itemModelIndex.data(ChatLineModel::DisplayRole).toString();
    uint foreground = itemModelIndex.data(Qt::ForegroundRole).toUInt();

    if (type == "header") {
        ChannelListHeader* header = qobject_cast<ChannelListHeader*>(listItem);
        header->updateItem(name, foreground);
    } else {
        ChannelListItem* item = qobject_cast<ChannelListItem*>(listItem);

        //QString text = "<html><span style='font-family:Courier New; font-size:18pt; color:" + foreground + ";'>" + name + "</span></html>";
        //qDebug() << "xxxxx ChannelListItemProvider::updateItem " << text << foreground;
        item->updateItem(name, foreground);

        if (item->actionSetCount())
            return;

        BufferId bufId = itemModelIndex.data(NetworkModel::BufferIdRole).value<BufferId>();
        ActionSet* actions = Bb10Ui::instance()->generateActionSetForBuffer(bufId);
        if (actions)
            item->addActionSet(actions);
        qDebug() << "xxxxx ChannelListItemProvider::updateItem " << name << actions->count();
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
    //textStyle.setColor(Color::LightGray);

    m_itemLabel = Label::create().text("").textStyle(textStyle);
    m_itemLabel->setVerticalAlignment(VerticalAlignment::Center);
    m_itemLabel->setMultiline(false);
    m_itemLabel->setLayoutProperties(StackLayoutProperties::create().spaceQuota(1));

    innerContainer->add(m_itemLabel);
    m_container->add(innerContainer);
    m_container->add(bottomBorder);

    setRoot(m_container);
}
void ChannelListHeader::updateItem(const QString text, const uint color)
{
    m_itemLabel->setText(text);
    m_itemLabel->textStyle()->setColor(Color::fromARGB(color));
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

    m_innerContainer = new Container();
    StackLayout *innerLayout = new StackLayout();
    innerLayout->setOrientation(LayoutOrientation::LeftToRight);
    m_innerContainer->setLayout(innerLayout);
    m_innerContainer->setVerticalAlignment(VerticalAlignment::Top);
    m_innerContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    m_innerContainer->setLeftPadding(55.0f);
    m_innerContainer->setPreferredHeight(68.0f);

    TextStyle style = SystemDefaults::TextStyles::titleText();
    m_itemLabel = Label::create().text("").textStyle(style);
    m_itemLabel->setVerticalAlignment(VerticalAlignment::Center);
    m_itemLabel->setHorizontalAlignment(HorizontalAlignment::Fill);
    m_itemLabel->setMultiline(false);
    m_itemLabel->setLayoutProperties(StackLayoutProperties::create().spaceQuota(1));
    m_innerContainer->add(m_itemLabel);

    m_container->add(bottomBorder);
    m_container->add(m_innerContainer);

    setRoot(m_container);
}
void ChannelListItem::updateItem(const QString text, const uint color)
{
    m_itemLabel->setText(text);
    m_itemLabel->textStyle()->setColor(Color::fromARGB(color));
}
void ChannelListItem::select(bool select)
{
    if (select)
        m_innerContainer->setBackground(Color::fromARGB(0xff323232));
    else
        m_innerContainer->resetBackground();
}
void ChannelListItem::reset(bool selected, bool activated)
{
    Q_UNUSED(selected);
    Q_UNUSED(activated);
    m_innerContainer->resetBackground();
}
void ChannelListItem::activate(bool activate)
{
    if (activate)
        m_innerContainer->setBackground(Color::fromARGB(0xff323232));
    else
        m_innerContainer->resetBackground();
}
