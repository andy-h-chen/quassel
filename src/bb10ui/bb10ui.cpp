#include "bb10ui.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/Application>
#include <bb/cascades/Container>
#include <bb/cascades/Divider>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Label>
#include <bb/cascades/LayoutOrientation>
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

#include "bb10uisettings.h"
#include "buffermodel.h"
#include "chatlinemodel.h"
#include "uimessageprocessor.h"

Bb10Ui::Bb10Ui(Application *app)
{
    bool connectResult;
    Q_UNUSED(connectResult);

    m_navPane = new NavigationPane();

    app->setScene(m_navPane);
}

void Bb10Ui::init()
{
    m_navPane->push(Page::create()
        .content(Label::create("First page")));

    connect(Client::instance(), SIGNAL(networkCreated(NetworkId)), SLOT(clientNetworkCreated(NetworkId)));
    connect(Client::coreConnection(), SIGNAL(startCoreSetup(QVariantList)), SLOT(showCoreConfigWizard(QVariantList)));
    connect(Client::coreConnection(), SIGNAL(connectionErrorPopup(QString)), SLOT(handleCoreConnectionError(QString)));
    connect(Client::coreConnection(), SIGNAL(coreSetupSuccess), SLOT(showCoreSetupSuccess));
    CoreConnection *conn = Client::coreConnection();
    if (!conn->connectToCore()) {
        qDebug() << "xxxxx !conn->connectToCore";
        // No autoconnect selected (or no accounts)
        //showCoreConnectionDlg();
    } else
        qDebug() << "xxxxx conn->connectToCore success!";
}

void Bb10Ui::clientNetworkCreated(NetworkId id)
{
    qDebug() << "xxxxx clientNetworkCreated id = " << id;
}

void Bb10Ui::showCoreConfigWizard(const QVariantList &backend)
{
    qDebug() << "xxxxx showCoreConfigWizard";
}

void Bb10Ui::showCoreSetupSuccess()
{
    qDebug() << "xxxxx showCoreSetupSuccess";
}

MessageModel *Bb10Ui::createMessageModel(QObject *parent)
{
    return new ChatLineModel(parent);
}

void Bb10Ui::handleCoreConnectionError(const QString &error)
{
    qDebug() << "xxxxx Core Connection Error " << error;
    //QMessageBox::critical(this, tr("Core Connection Error"), error, QMessageBox::Ok);
}

void Bb10Ui::connectedToCore()
{
    qDebug() << "xxxxx Bb10Ui::connectedToCore";
    setConnectedState();
}

void Bb10Ui::setConnectedState()
{
    // TODO: Update UI
    if (Client::networkIds().isEmpty()) {
        showNewIdentityPage();
    } else {
        // Monolithic always preselects last used buffer - Client only if the connection died
        if (Client::coreConnection()->wasReconnect() || Quassel::runMode() == Quassel::Monolithic) {
            Bb10UiSettings s;
            BufferId lastUsedBufferId(s.value("LastUsedBufferId").toInt());
            if (lastUsedBufferId.isValid())
                Client::bufferModel()->switchToBuffer(lastUsedBufferId);
        }
    }
}

void Bb10Ui::showNewIdentityPage()
{
    Page* identityPage = Page::create();
    // Set the title bar
    TitleBar* titleBar = TitleBar::create().visibility(ChromeVisibility::Visible).title("Identity and Server");
    identityPage->setTitleBar(titleBar);
    ActionItem* backAction = ActionItem::create();
    connect(backAction, SIGNAL(triggered()), this, SLOT(saveIdentityAndServer()));
    identityPage->setPaneProperties(NavigationPaneProperties::create().backButton(backAction));
    Container *mainContainer = Container::create();
    //layout->setOrientation(LayoutOrientation::TopToBottom);
    mainContainer->setLayout(DockLayout::create());
    identityPage->setContent(mainContainer);
    ScrollView* scrollView = ScrollView::create();
    mainContainer->add(scrollView);
    Container* container = Container::create();
    container->setLayout(StackLayout::create());
    scrollView->setContent(container);
    container->add(Label::create("Real Name:"));
    TextField *realname = new TextField();
    realname->setHintText("Enter your real name here...");
    realname->setHorizontalAlignment(HorizontalAlignment::Center);
    realname->setBottomMargin(20.0);
    container->add(realname);
    container->add(Label::create("Nick Name:"));
    TextField *nickname = new TextField();
    nickname->setHintText("Enter your nick name here...");
    nickname->setHorizontalAlignment(HorizontalAlignment::Center);
    nickname->setBottomMargin(20.0);
    container->add(nickname);
    container->add(Label::create("Alternative Nick Name:"));
    TextField *alternickname = new TextField();
    alternickname->setHintText("Enter your alternative nick name here...");
    alternickname->setHorizontalAlignment(HorizontalAlignment::Center);
    alternickname->setBottomMargin(50.0);
    container->add(alternickname);
    container->add(Label::create("Away message:"));
    TextField *awaymsg = new TextField();
    awaymsg->setHintText("Enter your away message here...");
    awaymsg->setHorizontalAlignment(HorizontalAlignment::Center);
    awaymsg->setBottomMargin(50.0);
    container->add(awaymsg);

    container->add(Divider::create());
    container->add(Label::create("Server address:"));
    TextField *serveraddr = new TextField();
    serveraddr->setHintText("Enter your server address here...");
    serveraddr->setHorizontalAlignment(HorizontalAlignment::Center);
    serveraddr->setBottomMargin(20.0);
    container->add(serveraddr);
    container->add(Label::create("Port:"));
    TextField *port = new TextField();
    port->setHintText("Enter your server port here...");
    port->setHorizontalAlignment(HorizontalAlignment::Center);
    port->setBottomMargin(20.0);
    container->add(port);
    container->add(Label::create("Password:"));
    TextField *password = new TextField();
    password->setHintText("Enter your server password here...");
    password->setHorizontalAlignment(HorizontalAlignment::Center);
    password->setBottomMargin(20.0);
    container->add(password);

    container->add(Label::create("Join Channels Automatically:"));
    TextArea *autojoin = new TextArea();
    autojoin->setHintText("Auto join channels ...");
    autojoin->setMinHeight(120.0f);
    autojoin->setMaxHeight(200.0f);
    autojoin->setPreferredHeight(0);
    autojoin->setBottomMargin(50.0);
    autojoin->textStyle()->setBase(SystemDefaults::TextStyles::bodyText());
    autojoin->setHorizontalAlignment(HorizontalAlignment::Fill);
    container->add(autojoin);

    m_navPane->push(identityPage);
}

void Bb10Ui::saveIdentityAndServer()
{
    qDebug() << "xxxxx Bb10Ui::saveIdentityAndServer";
    m_navPane->pop();
}

AbstractMessageProcessor *Bb10Ui::createMessageProcessor(QObject *parent)
{
    return new UiMessageProcessor(parent);
}
