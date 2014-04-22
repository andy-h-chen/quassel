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
private slots:
    void onKeyPressHandler(KeyEvent*);
    void sendMessage();
private:
    Page* m_page;
    TextField* m_input;
    Button* m_sendButton;
    ListView* m_chatListView;
    BufferId m_id;
    QString m_name;
};

#endif

