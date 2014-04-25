#ifndef _NICKLISTITEMPROVIDER_H_
#define _NICKLISTITEMPROVIDER_H_

#include <bb/cascades/ListItemProvider>

using namespace bb::cascades;

namespace bb
{
  namespace cascades
  {
    class ListView;
  }
}

class NickListItemProvider: public bb::cascades::ListItemProvider
{
public:
    NickListItemProvider();

    VisualNode * createItem(ListView* list, const QString &type);
    void updateItem(ListView* list, VisualNode *listItem, const QString &type, const QVariantList &indexPath, const QVariant &data);
};

#endif // ifndef _NICKLISTITEMPROVIDER_H_
