#ifndef _NICKLISTITEM_H_
#define _NICKLISTITEM_H_

#include <bb/cascades/CustomControl>
#include <bb/cascades/ListItemListener>

using namespace bb::cascades;

namespace bb
{
  namespace cascades
  {
    class ImageView;
    class Label;
    class Container;
  }
}

class NickListItem: public bb::cascades::CustomControl, public ListItemListener
{
  Q_OBJECT

public:
    NickListItem(Container *parent = 0);

  void updateItem(const QString text);
  void select(bool select);
  void reset(bool selected, bool activated);
  void activate(bool activate);

private:
  Label *mItemLabel;
  Container *mHighlighContainer;
};

class NickListHeader: public bb::cascades::CustomControl, public ListItemListener
{
  Q_OBJECT

public:
    NickListHeader(Container *parent = 0);

  void updateItem(const QString text);
  void select(bool select);
  void reset(bool selected, bool activated);
  void activate(bool activate);

private:
  Label *mItemLabel;
  Container *mHighlighContainer;
};
#endif // ifndef _NICKLISTITEM_H_
