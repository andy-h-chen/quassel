#include "chatlineprovider.h"

#include <bb/cascades/ListView>
#include <bb/cascades/ScrollAnimation>
#include <bb/cascades/ScrollPosition>

#include "chatline.h"
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

    QModelIndex msgIndex = static_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, 2);

    //Message message = msgIndex.data(MessageModel::MessageRole).value<Message>();
    int flags = msgIndex.data(MessageModel::FlagsRole).value<int>();

    QModelIndex contentsModelIndex = static_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, (int)ChatLineModel::ContentsColumn);
    QModelIndex senderModelIndex = static_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, (int)ChatLineModel::SenderColumn);
    ChatLine *line = static_cast<ChatLine*>(listItem);
    QString sender = senderModelIndex.data(ChatLineModel::DisplayRole).toString();
    QString contents = contentsModelIndex.data(ChatLineModel::DisplayRole).toString();
    contents.replace("<", "&lt;").replace(">", "&gt;").replace("&", "&amp;");
    sender.replace("<", "&lt;").replace(">", "&gt;").replace("&", "&amp;");
    QString msg = "<html><span style='font-family:Courier New; color:orange'>" + sender + "</span> <span style='font-family:Courier New;'>" + contents + "</span></html>";
    //qDebug() << "xxxxx ChatLineProvider::updateItem flags&Message::Highlight = " << (flags & Message::Highlight);
    line->updateItem(msg);
    if (flags & Message::Highlight)
        line->setHighlight(true);
    if (m_scrollToNewLine) {
        list->scrollToPosition(ScrollPosition::End, ScrollAnimation::Default);
        m_scrollToNewLine = false;
    }
}
