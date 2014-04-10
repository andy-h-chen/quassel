#ifndef CHANNELLISTDATAMODEL_H
#define CHANNELLISTDATAMODEL_H

#include <bb/cascades/DataModel>

class ChannelListDataModel : public bb::cascades::DataModel
{
    Q_OBJECT
public:
    ChannelListDataModel(QObject *parent = 0);

    // Required interface implementation
    virtual int childCount(const QVariantList& indexPath);
    virtual bool hasChildren(const QVariantList& indexPath);
    virtual QVariant data(const QVariantList& indexPath);
    virtual QString itemType(const QVariantList& indexPath);
};

#endif
