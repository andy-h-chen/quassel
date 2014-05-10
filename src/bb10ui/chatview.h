#ifndef CHATVIEW_H_
#define CHATVIEW_H_

#include <QObject>

#include <bb/cascades/TextField>

#include "network.h"

using namespace bb::cascades;

namespace bb
{
    namespace cascades
    {
        class Button;
        class KeyEvent;
        class ListView;
        class Page;
        class TextField;
    }
}


class ChatView : public QObject
{
    Q_OBJECT
public:
    ChatView(BufferId, QString&);
    ~ChatView();

    Page* getPage() { return m_page; }
    MsgId getLastMsgId() const { return m_lastMsgId; }
private slots:
    void onKeyPressHandler(bb::cascades::KeyEvent*);
    void sendMessage();
    void showNicks();
    void onNickListTriggered(const QVariantList);
    void queryNick();
    void onItemAdded(const QVariantList);
private:
    Page* m_page;
    TextField* m_input;
    Button* m_sendButton;
    ListView* m_chatListView;
    BufferId m_id;
    QString m_name;
    MsgId m_lastMsgId;

    Page* m_nickListPage;
    ListView* m_nickListView;
};

#endif

