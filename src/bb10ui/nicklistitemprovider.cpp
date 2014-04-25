#include "nicklistitemprovider.h"

#include <bb/cascades/ListView>
#include <bb/cascades/ScrollAnimation>
#include <bb/cascades/ScrollPosition>

#include "chatline.h"
#include "chatlinemodel.h"
#include "datamodeladapter.h"
#include "networkmodel.h"
#include "nicklistitem.h"

using namespace bb::cascades;

NickListItemProvider::NickListItemProvider()
{
}

VisualNode * NickListItemProvider::createItem(ListView* list, const QString &type)
{
    Q_UNUSED(list);
    qDebug() << "xxxxx NickListItemProvider::createItem type = " << type;
    if (type == "header")
        return new NickListHeader();
    else
        return new NickListItem();
}

void NickListItemProvider::updateItem(ListView* list, bb::cascades::VisualNode *listItem,
    const QString &type, const QVariantList &indexPath, const QVariant &data)
{
    Q_UNUSED(data);
    QModelIndex index = static_cast<DataModelAdapter*>(list->dataModel())->getQModelIndex(indexPath, 0);
    qDebug() << "xxxxx NickListItemProvider::updateItem indexPath = " << indexPath << " data = " << index.data(Qt::DisplayRole).toString();
    if (type == "header") {
        NickListHeader* item = static_cast<NickListHeader*>(listItem);
        item->updateItem(index.data(Qt::DisplayRole).toString());
    } else {
        NickListItem* item = static_cast<NickListItem*>(listItem);
        item->updateItem(index.data(Qt::DisplayRole).toString());
    }
}
