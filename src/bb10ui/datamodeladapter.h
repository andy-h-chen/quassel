#ifndef CHANNELLISTDATAMODEL_H
#define CHANNELLISTDATAMODEL_H

#include <bb/cascades/DataModel>

class QModelIndex;
class QSortFilterProxyModel;

class DataModelAdapter : public bb::cascades::DataModel
{
    Q_OBJECT
public:
    DataModelAdapter(QAbstractItemModel* model, QVariantList startPoint = QVariantList(), QObject *parent = 0);

    // Required interface implementation
    virtual int childCount(const QVariantList& indexPath);
    virtual bool hasChildren(const QVariantList& indexPath);
    virtual QVariant data(const QVariantList& indexPath);
    virtual QString itemType(const QVariantList& indexPath);

    QModelIndex getQModelIndex(QVariantList list, int column = 0) const;
private slots:
    void handleBufferModelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
    void handleBufferModelRowsRemoved(const QModelIndex & parent, int start, int end);
    void handleBufferModelRowsInserted(const QModelIndex & parent, int start, int end);
    void handleLayoutChanged();
private:
    QModelIndex getStartIndex() const;
    QAbstractItemModel* m_model;
    QVariantList m_startPoint;
};

#endif
