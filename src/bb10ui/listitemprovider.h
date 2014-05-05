#ifndef _LISTITEMPROVIDER_H_
#define _LISTITEMPROVIDER_H_

#include <bb/cascades/CustomControl>
#include <bb/cascades/ListItemListener>
#include <bb/cascades/ListItemProvider>

using namespace bb::cascades;

namespace bb
{
  namespace cascades
  {
    class ImageView;
    class Label;
    class Container;
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

class ChatLine: public bb::cascades::CustomControl, public ListItemListener
{
  Q_OBJECT

public:
    ChatLine(Container *parent = 0);
    void updateItem(const QString text);
    void select(bool select);
    void reset(bool selected, bool activated);
    void activate(bool activate);
    void setHighlight(bool b);
private:
    Label* m_itemLabel;
    Container* m_container;
};

class ChannelListItemProvider : public bb::cascades::ListItemProvider
{
public:
    ChannelListItemProvider();
    VisualNode * createItem(ListView* list, const QString &type);
    void updateItem(ListView* list, VisualNode *listItem, const QString &type, const QVariantList &indexPath, const QVariant &data);
};

class ChannelListHeader : public bb::cascades::CustomControl, public ListItemListener
{
    Q_OBJECT
public:
    ChannelListHeader(Container* parent = 0);
    void updateItem(const QString text);
    void select(bool select);
    void reset(bool selected, bool activated);
    void activate(bool activate);
private:
    Label* m_itemLabel;
    Container* m_container;
};

class ChannelListItem : public bb::cascades::CustomControl, public ListItemListener
{
    Q_OBJECT
public:
    ChannelListItem(Container* parent = 0);
    void updateItem(const QString text);
    void select(bool select);
    void reset(bool selected, bool activated);
    void activate(bool activate);
private:
    Label* m_itemLabel;
    Container* m_container;
};

#endif // ifndef _LISTITEMPROVIDER_H_
