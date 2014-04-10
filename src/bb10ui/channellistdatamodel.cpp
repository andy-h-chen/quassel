#include "channellistdatamodel.h"

#include "buffermodel.h"
#include "client.h"

ChannelListDataModel::ChannelListDataModel(QObject *parent)
    : bb::cascades::DataModel(parent)
{

}

int ChannelListDataModel::childCount(const QVariantList& indexPath)
{
    BufferModel* bm = Client::bufferModel();
    const int level = indexPath.size();

    if (level == 0) {
        return bm->rowCount();
    }

    if (level == 1) { // The number of child items for a header is requested
        const int header = indexPath[0].toInt();
        return bm->rowCount(bm->index(header, 0));
    }

    // The number of child items for 2nd level items is requested -> always 0
    return 0;
}

bool ChannelListDataModel::hasChildren(const QVariantList& indexPath)
{
    BufferModel* bm = Client::bufferModel();
    const int level = indexPath.size();

    if (level == 0) {
        return bm->hasChildren();
    }

    if (level == 1) {
        const int header = indexPath[0].toInt();
        return bm->hasChildren(bm->index(header, 0));
    }

    return false;
}

/*
 * Return data as a string QVariant for any requested indexPath.
 * We could add defensive code to ensure that the data is not
 * out of range, but ListView honours the results of hasChildren
 * and childCount.
 */
//! [3]
QVariant ChannelListDataModel::data(const QVariantList& indexPath)
{
    QVariant value;
    BufferModel* bm = Client::bufferModel();
    if (indexPath.size() == 1) { // Header requested
        if (indexPath[0].toInt() < bm->rowCount())
            value = bm->data(bm->index(indexPath[0].toInt(), 0));
    }

    if (indexPath.size() == 2) { // 2nd-level item requested
        const int header = indexPath[0].toInt();
        const int childItem = indexPath[1].toInt();
        if (header < bm->rowCount() && childItem < bm->rowCount(bm->index(header, 0)))
            value = bm->data(bm->index(header, 0).child(childItem, 0));
    }

    qDebug() << "Data for " << indexPath << " is " << value;

    return value;
}

QString ChannelListDataModel::itemType(const QVariantList& indexPath)
{
    switch (indexPath.size()) {
        case 0:
            return QString();
            break;
        case 1:
            return QLatin1String("header");
            break;
        default:
            return QLatin1String("item");
            break;
    }
}

