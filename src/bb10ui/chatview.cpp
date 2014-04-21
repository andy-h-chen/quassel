#include "chatview.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/Container>
#include <bb/cascades/Divider>
#include <bb/cascades/DockLayout>
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

#include <bb/cascades/resources/titlebar.h>
#include <bb/cascades/controls/chromevisibility.h>

#include "bb10ui.h"
#include "datamodeladapter.h"
#include "messagefilter.h"

ChatView::ChatView(BufferId id, QString& name)
    : QObject(0)
    , m_id(id)
    , m_name(name)
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
    m_chatListView->setDataModel(dataModelAdapter);
    mainContainer->add(m_chatListView);

    m_input = new TextField();
    m_input->setHorizontalAlignment(HorizontalAlignment::Fill);
    m_input->setVerticalAlignment(VerticalAlignment::Bottom);
    mainContainer->add(m_input);

    ActionItem* nickList = ActionItem::create()
    .title("Nick List");
    //connect(saveAndConnect, SIGNAL(triggered()), Bb10Ui::instance(), SLOT(saveIdentityAndServer()));
    m_page->addAction(nickList, ActionBarPlacement::OnBar);

    ActionItem* backAction = ActionItem::create();
    //connect(backAction, SIGNAL(triggered()), Bb10Ui::instance(), SLOT(saveIdentityAndServer()));
    m_page->setPaneProperties(NavigationPaneProperties::create().backButton(backAction));
}

ChatView::~ChatView()
{
    delete m_chatListView;
    delete m_input;
    delete m_page;
}

