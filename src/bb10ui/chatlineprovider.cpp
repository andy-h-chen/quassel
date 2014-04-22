#include "chatlineprovider.h"

#include <bb/cascades/ListView>
#include <bb/cascades/ScrollAnimation>
#include <bb/cascades/ScrollPosition>

#include "chatline.h"
#include "chatlinemodel.h"
#include "datamodeladapter.h"

using namespace bb::cascades;

ChatLineProvider::ChatLineProvider()
{
}

VisualNode * ChatLineProvider::createItem(ListView* list, const QString &type)
{
    Q_UNUSED(type);
    Q_UNUSED(list);

    ChatLine *line = new ChatLine();
    return line;
}

void ChatLineProvider::updateItem(ListView* list, bb::cascades::VisualNode *listItem,
    const QString &type, const QVariantList &indexPath, const QVariant &data)
{
    Q_UNUSED(list);
    Q_UNUSED(type);

    QModelIndex contentsModelIndex = static_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, (int)ChatLineModel::ContentsColumn);
    QModelIndex senderModelIndex = static_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, (int)ChatLineModel::SenderColumn);
    ChatLine *line = static_cast<ChatLine*>(listItem);
    QString msg = "<html><span style='font-family:Courier New; color:orange'>" + senderModelIndex.data(ChatLineModel::DisplayRole).toString().replace("<", "&lt;").replace(">", "&gt;") + "</span> <span style='font-family:Courier New;'>" + contentsModelIndex.data(ChatLineModel::DisplayRole).toString().replace("<", "&lt;").replace(">", "&gt;") + "</span></html>";
    line->updateItem(msg);
    list->scrollToPosition(ScrollPosition::End, ScrollAnimation::Default);
}
