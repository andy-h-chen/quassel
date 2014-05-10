#include "datamodeladapter.h"

#include <QSortFilterProxyModel>

#include "buffermodel.h"
#include "client.h"

DataModelAdapter::DataModelAdapter(QAbstractItemModel* model, QVariantList startPoint, QObject *parent)
    : bb::cascades::DataModel(parent)
    , m_model(model)
    , m_startPoint(startPoint)
{
    connect(m_model, SIGNAL(rowsRemoved (const QModelIndex&, int, int)), this, SLOT(handleBufferModelRowsRemoved(const QModelIndex &, int, int)));
    connect(m_model, SIGNAL(rowsInserted (const QModelIndex&, int, int)), this, SLOT(handleBufferModelRowsInserted(const QModelIndex &, int, int)));
    connect(m_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(handleBufferModelDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(m_model, SIGNAL(layoutChanged()), this, SLOT(handleLayoutChanged()));
}

int DataModelAdapter::childCount(const QVariantList& indexPath)
{
    const int level = indexPath.size();
    QModelIndex startIndex = getStartIndex();
    //qDebug() << "xxxxx DataModelAdapter::childCount startIndex = " << startIndex << startIndex.isValid() << m_startPoint.size();
    if (level == 0) {
        return startIndex.isValid() ? m_model->rowCount(startIndex) : m_model->rowCount();
    }

    if (level == 1) { // The number of child items for a header is requested
        const int header = indexPath[0].toInt();
        return startIndex.isValid() ? m_model->rowCount(startIndex.child(header, 0)) : m_model->rowCount(m_model->index(header, 0));
    }

    // The number of child items for 2nd level items is requested -> always 0
    return 0;
}

bool DataModelAdapter::hasChildren(const QVariantList& indexPath)
{
    const int level = indexPath.size();
    QModelIndex startIndex = getStartIndex();
    if (level == 0) {
        return startIndex.isValid() ? m_model->hasChildren(startIndex) : m_model->hasChildren();
    }

    if (level == 1) {
        const int header = indexPath[0].toInt();
        return startIndex.isValid() ? m_model->hasChildren(startIndex.child(header, 0)) : m_model->hasChildren(m_model->index(header, 0));
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
QVariant DataModelAdapter::data(const QVariantList& indexPath)
{
    QVariant value;
    QModelIndex startIndex = getStartIndex();
    if (indexPath.size() == 1) { // Header requested
        //if (indexPath[0].toInt() < m_sortFilterProxyModel->rowCount())
        value = startIndex.isValid() ? m_model->data(startIndex.child(indexPath[0].toInt(), 0)) : m_model->data(m_model->index(indexPath[0].toInt(), 0));
    }

    if (indexPath.size() == 2) { // 2nd-level item requested
        const int header = indexPath[0].toInt();
        const int childItem = indexPath[1].toInt();
        //if (header < m_sortFilterProxyModel->rowCount() && childItem < m_sortFilterProxyModel->rowCount(m_sortFilterProxyModel->index(header, 0)))
        value = startIndex.isValid() ? m_model->data(startIndex.child(header, 0).child(childItem, 0)) : m_model->data(m_model->index(header, 0).child(childItem, 0));
    }

    //qDebug() << "xxxxx DataModelAdapter::data Data for " << indexPath << " is " << value;

    return value;
}

QString DataModelAdapter::itemType(const QVariantList& indexPath)
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

void DataModelAdapter::handleLayoutChanged()
{
    qDebug() << "xxxxx DataModelAdapter::handleLayoutChanged";
    emit itemsChanged(bb::cascades::DataModelChangeType::AddRemove); 
}

void DataModelAdapter::handleBufferModelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
    // Ignore the change other than the 1st column.
    if (topLeft.column() >= 1 || bottomRight.column() >=1 || topLeft.parent().row() != bottomRight.parent().row())
        return;

    int header = topLeft.parent().row();
    for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
        QVariantList list;
        if (header != -1)
            list.push_back(header);
        list.push_back(i);
        emit itemUpdated(list);
    }
}

void DataModelAdapter::handleBufferModelRowsRemoved(const QModelIndex & parent, int start, int end)
{
    qDebug() << "xxxxx ChannelListDataModel::handleBufferModelRowsRemoved parent = " << parent << start << end;
    if (parent.column() >= 1)
        return;

    for (int i = start; i <= end; i++) {
        QVariantList list;
        if (parent.row() != -1)
            list.push_back(parent.row());
        list.push_back(i);
        emit itemRemoved(list);
    }
}

void DataModelAdapter::handleBufferModelRowsInserted(const QModelIndex & parent, int start, int end)
{
    //qDebug() << "xxxxx DataModelAdapter::handleBufferModelRowsInserted parent = " << parent << start << end;
    if (parent.column() >= 1)
        return;

    for (int i = start; i <= end; i++) {
        QVariantList list;
        // parent.row == -1 means header
        if (parent.row() != -1)
            list.push_back(parent.row());
        list.push_back(i);
        //qDebug() << "xxxxx DataModelAdapter::handleBufferModelRowsInserted list = " << list;
        emit itemAdded(list);
    }
}


QModelIndex DataModelAdapter::getQModelIndex(QVariantList list, int column) const
{
    QModelIndex startIndex = getStartIndex();
    if (list.length() == 1)
        return startIndex.isValid() ? startIndex.child(list[0].toInt(), column) : m_model->index(list[0].toInt(), column);

    if (list.length() == 2)
        return startIndex.isValid() ? startIndex.child(list[0].toInt(), column).child(list[1].toInt(), column) : m_model->index(list[0].toInt(), column).child(list[1].toInt(), column);

    return QModelIndex();
}

QVariantList DataModelAdapter::getDataModelIndex(QModelIndex index) const
{
    if (!index.isValid())
        return QVariantList();

    QVariantList list;
    if (index.parent().row() != -1)
        list.push_back(index.parent().row());
    list.push_back(index.row());
    return list;
}

QModelIndex DataModelAdapter::getStartIndex() const
{
    QModelIndex startIndex = m_model->index(-1, -1);
    if (m_startPoint.size()) {
        startIndex = m_model->index(m_startPoint[0].toInt(), 0);
        for (int i=1; i<m_startPoint.size(); i++)
            startIndex = startIndex.child(m_startPoint[i].toInt(), 0);
    }
    return startIndex;
}
