#ifndef _LISTITEMPROVIDER_H_
#define _LISTITEMPROVIDER_H_

#include <bb/cascades/ListItemProvider>

using namespace bb::cascades;

namespace bb
{
  namespace cascades
  {
    class ListView;
  }
}

class ChatLineProvider: public bb::cascades::ListItemProvider
{
public:
    ChatLineProvider();

    VisualNode * createItem(ListView* list, const QString &type);
    void updateItem(ListView* list, VisualNode *listItem, const QString &type, const QVariantList &indexPath, const QVariant &data);
private:
    bool m_scrollToNewLine;
};

#endif // ifndef _LISTITEMPROVIDER_H_
