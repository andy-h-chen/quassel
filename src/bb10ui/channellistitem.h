#ifNdef CHANNELLISTITEM_H
#define CHANNELLISTITEM_H

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

class ChannelListItem: public CustomControl, public ListItemListener
{
    Q_OBJECT
public:
    ChannelListItem(Container *parent = 0);
    void updateItem(const QString text);
    void select(bool select);
    void reset(bool selected, bool activated);
    void activate(bool activate);
private:
    Label *m_itemLabel;
    Container *m_highlighContainer;
};

#endif
