#include "chatview.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/ActionSet>
#include <bb/cascades/Button>
#include <bb/cascades/Container>
#include <bb/cascades/Divider>
#include <bb/cascades/DockLayout>
#include <bb/cascades/KeyEvent>
#include <bb/cascades/KeyListener>
#include <bb/cascades/Label>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/ListView>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/NavigationPaneProperties>
#include <bb/cascades/Page>
#include <bb/cascades/ScrollView>
#include <bb/cascades/StackLayout>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/TextArea>
#include <bb/cascades/TextField>
#include <bb/cascades/TextFieldInputMode>

#include <bb/cascades/resources/titlebar.h>
#include <bb/cascades/controls/chromevisibility.h>

#include "bb10ui.h"
#include "listitemprovider.h"
#include "datamodeladapter.h"
#include "messagefilter.h"
#include "nicklistitemprovider.h"
#include "nickviewfilter.h"

ChatView::ChatView(BufferId id, QString& name)
    : QObject(0)
    , m_id(id)
    , m_name(name)
    , m_lastMsgId(-1)
    , m_nickListPage(0)
    , m_nickListView(0)
{
    QList<BufferId> filterList;
    filterList.append(id);
    MessageFilter *filter = new MessageFilter(Client::messageModel(), filterList, this);
    DataModelAdapter* dataModelAdapter = new DataModelAdapter(filter);

    m_page = Page::create();
    // Set the title bar
    TitleBar* titleBar = TitleBar::create().visibility(ChromeVisibility::Visible).title(m_name);
    m_page->setTitleBar(titleBar);

    Container *mainContainer = Container::create();
    //layout->setOrientation(LayoutOrientation::TopToBottom);
    mainContainer->setLayout(StackLayout::create());
    m_page->setContent(mainContainer);

    m_chatListView = new ListView();
    m_chatListView->setHorizontalAlignment(HorizontalAlignment::Fill);
    m_chatListView->setDataModel(dataModelAdapter);
    m_chatListView->setListItemProvider(new ChatLineProvider());
    mainContainer->add(m_chatListView);

    Container* inputContainer = Container::create();
    inputContainer->setLayout(DockLayout::create());
    inputContainer->setVerticalAlignment(VerticalAlignment::Bottom);
    inputContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    //inputContainer->setPreferredHeight(80.0f);
    m_input = new TextField();
    m_input->setHorizontalAlignment(HorizontalAlignment::Left);
    m_input->setVerticalAlignment(VerticalAlignment::Fill);
    m_input->setInputMode(TextFieldInputMode::Chat);
    inputContainer->add(m_input);
    m_sendButton = Button::create().text("Send");
    m_sendButton->setHorizontalAlignment(HorizontalAlignment::Right);
    m_sendButton->setVerticalAlignment(VerticalAlignment::Fill);
    m_sendButton->setPreferredWidth(100.0f);
    inputContainer->add(m_sendButton);
    connect(m_sendButton, SIGNAL(clicked()), this, SLOT(sendMessage()));
    mainContainer->add(inputContainer);

    KeyListener* inputKeyListener = KeyListener::create()
        .onKeyPressed(this, SLOT(onKeyPressHandler(bb::cascades::KeyEvent*)));
    m_input->addKeyListener(inputKeyListener);

    ActionItem* nickList = ActionItem::create()
    .title("Nick List").image(Image("icons/nicklist.png"));
    connect(nickList, SIGNAL(triggered()), this, SLOT(showNicks()));
    m_page->addAction(nickList, ActionBarPlacement::OnBar);

    ActionItem* backAction = ActionItem::create();
    connect(backAction, SIGNAL(triggered()), Bb10Ui::instance(), SLOT(navPanePop()));
    m_page->setPaneProperties(NavigationPaneProperties::create().backButton(backAction));

    connect(dataModelAdapter, SIGNAL(itemAdded(QVariantList)), this, SLOT(onItemAdded(QVariantList)));
}

ChatView::~ChatView()
{
    delete m_chatListView;
    delete m_input;
    delete m_sendButton;
    delete m_page;
}

void ChatView::onKeyPressHandler(bb::cascades::KeyEvent* event)
{
    qDebug() << "xxxxx ChatView::onKeyEventHandler key = " << event->key();
    if (event->key() == 13)
        sendMessage();
}

void ChatView::sendMessage()
{
    if (m_input->text().length()) {
        Client::userInput(Client::networkModel()->bufferInfo(m_id), m_input->text());
        m_input->setText("");
    }
}

void ChatView::showNicks()
{
    if (!m_nickListPage) {
        m_nickListPage = Page::create();
        TitleBar* titleBar = TitleBar::create().visibility(ChromeVisibility::Visible).title("Nick List for " + m_name);
        m_nickListPage->setTitleBar(titleBar);
        m_nickListView = new ListView();
        m_nickListView->setHorizontalAlignment(HorizontalAlignment::Fill);
        NickViewFilter* nickViewFilter = new NickViewFilter(m_id, Client::networkModel());
        QVariantList startPoint;
        for (int i=0; i<nickViewFilter->rowCount() && !startPoint.size(); i++) {  // loop thru networks
            for (int j=0; j<nickViewFilter->rowCount(nickViewFilter->index(i, 0)) && !startPoint.size(); j++) { // loop thru channels
                if (nickViewFilter->data(nickViewFilter->index(i, 0).child(j, 0), NetworkModel::BufferIdRole).value<BufferId>() == m_id) {
                    startPoint.push_back(i);
                    startPoint.push_back(j);
                    qDebug() << "xxxxx ChatView::showNicks startPoint" << startPoint << " m_id = " << m_id;
                }
            }
        }
        nickViewFilter->sort(0);
        m_nickListView->setDataModel(new DataModelAdapter(nickViewFilter, startPoint));
        //m_nickListView->setListItemProvider(new NickListItemProvider());
        m_nickListPage->setContent(m_nickListView);
        connect(m_nickListView, SIGNAL(triggered(const QVariantList)), this, SLOT(onNickListTriggered(const QVariantList)));
        ActionItem* query = ActionItem::create().title("Query").image(Image("icons/query.png"));
        ActionSet* actions = ActionSet::create();
        actions->add(query);
        m_nickListView->addActionSet(actions);
        connect(query, SIGNAL(triggered()), this, SLOT(queryNick()));
    }
    Bb10Ui::instance()->navPanePush(m_nickListPage);
}

void ChatView::onNickListTriggered(const QVariantList index)
{
    QModelIndex modelIndex = static_cast<DataModelAdapter*>(m_nickListView->dataModel())->getQModelIndex(index);
    QString nick = modelIndex.data(Qt::DisplayRole).value<QString>();
    qDebug() << "xxxxx ChatView::onNickListTriggered nick = " << nick;
    QString text = m_input->text();
    if (!text.length())
        m_input->setText(nick + ": ");
    else if (text.at(text.length()) == ' ')
        m_input->setText(text + nick);
    else
        m_input->setText(text + ' ' + nick);
    Bb10Ui::instance()->navPanePop();
    m_input->requestFocus();
}

void ChatView::queryNick()
{
    QVariantList index = m_nickListView->selected();
    if (index.length() < 2)
        return;

    QModelIndex modelIndex = static_cast<DataModelAdapter*>(m_nickListView->dataModel())->getQModelIndex(index);
    QString nick = modelIndex.data(Qt::DisplayRole).value<QString>();
    qDebug() << "xxxxx ChatView::queryNick selected = " << index;
    Bb10Ui::instance()->switchToOrJoinChat(nick, true);
}

void ChatView::onItemAdded(const QVariantList indexPath)
{
    QModelIndex msgIndex = qobject_cast<DataModelAdapter*>(m_chatListView->dataModel())->getQModelIndex(indexPath, 2);
    m_lastMsgId = msgIndex.data(MessageModel::MsgIdRole).value<MsgId>();
}
