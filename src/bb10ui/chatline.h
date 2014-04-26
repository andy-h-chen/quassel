#ifndef _CHATLINE_H_
#define _CHATLINE_H_

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

#endif // ifndef _CHATLINE_H_
