#include "simplesetuppage.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/CheckBox>
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
#include <bb/cascades/TextFieldInputMode>

#include <bb/cascades/resources/titlebar.h>
#include <bb/cascades/controls/chromevisibility.h>

#include "bb10ui.h"
#include "clientidentity.h"

SimpleSetupPage::SimpleSetupPage(QObject* parent)
    : QObject(parent)
{
    m_page = Page::create();
    // Set the title bar
    TitleBar* titleBar = TitleBar::create().visibility(ChromeVisibility::Visible).title("Identity and Server");
    m_page->setTitleBar(titleBar);

    Container *mainContainer = Container::create();
    //layout->setOrientation(LayoutOrientation::TopToBottom);
    mainContainer->setLayout(DockLayout::create());
    m_page->setContent(mainContainer);
    ScrollView* scrollView = ScrollView::create();
    mainContainer->add(scrollView);
    Container* container = Container::create();
    container->setLayout(StackLayout::create());
    scrollView->setContent(container);
    container->add(Label::create("Real Name:"));
    m_realName = new TextField();
    m_realName->setHintText("Enter your real name here...");
    m_realName->setHorizontalAlignment(HorizontalAlignment::Center);
    m_realName->setBottomMargin(20.0);
    container->add(m_realName);

    container->add(Label::create("Nick Name:"));
    m_nickName = new TextField();
    m_nickName->setHintText("Enter your nick name here...");
    m_nickName->setHorizontalAlignment(HorizontalAlignment::Center);
    m_nickName->setBottomMargin(20.0);
    container->add(m_nickName);
    container->add(Label::create("Alternative Nick Name:"));
    m_alterNick = new TextField();
    m_alterNick->setHintText("Enter your alternative nick name here...");
    m_alterNick->setHorizontalAlignment(HorizontalAlignment::Center);
    m_alterNick->setBottomMargin(50.0);
    container->add(m_alterNick);

    container->add(Label::create("Away message:"));
    m_awayMessage = new TextField();
    m_awayMessage->setHintText("Enter your away message here...");
    m_awayMessage->setHorizontalAlignment(HorizontalAlignment::Center);
    m_awayMessage->setBottomMargin(50.0);
    container->add(m_awayMessage);

    container->add(Divider::create());

    container->add(Label::create("Network name:"));
    m_networkName = new TextField();
    m_networkName->setHintText("Enter your network name here...");
    m_networkName->setHorizontalAlignment(HorizontalAlignment::Center);
    m_networkName->setBottomMargin(20.0);
    container->add(m_networkName);

    container->add(Label::create("Server address:"));
    m_serverAddr = new TextField();
    m_serverAddr->setHintText("Enter your server address here...");
    m_serverAddr->setHorizontalAlignment(HorizontalAlignment::Center);
    m_serverAddr->setInputMode(TextFieldInputMode::Url);
    m_serverAddr->setBottomMargin(20.0);
    container->add(m_serverAddr);

    container->add(Label::create("Port:"));
    m_port = new TextField();
    m_port->setHintText("Enter your server port here...");
    m_port->setHorizontalAlignment(HorizontalAlignment::Center);
    m_port->setInputMode(TextFieldInputMode::NumbersAndPunctuation);
    m_port->setBottomMargin(20.0);
    container->add(m_port);

    container->add(Label::create("Password:"));
    m_password = new TextField();
    m_password->setHintText("Enter your server password here...");
    m_password->setHorizontalAlignment(HorizontalAlignment::Center);
    m_password->setInputMode(TextFieldInputMode::Password);
    m_password->setBottomMargin(20.0);
    container->add(m_password);

    m_useSSL = CheckBox::create().text("Use SSL");
    container->add(m_useSSL);

    container->add(Label::create("Join Channels Automatically:"));
    m_autojoin = new TextArea();
    m_autojoin->setHintText("Auto join channels ...");
    m_autojoin->setMinHeight(120.0f);
    m_autojoin->setMaxHeight(200.0f);
    m_autojoin->setPreferredHeight(0);
    m_autojoin->setBottomMargin(50.0);
    m_autojoin->textStyle()->setBase(SystemDefaults::TextStyles::bodyText());
    m_autojoin->setHorizontalAlignment(HorizontalAlignment::Fill);
    container->add(m_autojoin);

    ActionItem* saveAndConnect = ActionItem::create()
    .title("Save & Connect");
    connect(saveAndConnect, SIGNAL(triggered()), Bb10Ui::instance(), SLOT(saveIdentityAndServer()));
    m_page->addAction(saveAndConnect, ActionBarPlacement::OnBar);

    ActionItem* backAction = ActionItem::create();
    connect(backAction, SIGNAL(triggered()), Bb10Ui::instance(), SLOT(saveIdentityAndServer()));
    m_page->setPaneProperties(NavigationPaneProperties::create().backButton(backAction));
    qDebug() << "xxxxx SimpleSetupPage::SimpleSetupPage init done";
}

SimpleSetupPage::~SimpleSetupPage()
{
    delete m_page;
    delete m_realName;
    delete m_nickName;
    delete m_alterNick;
    delete m_awayMessage;
    delete m_serverAddr;
    delete m_port;
    delete m_password;
    delete m_useSSL;
    delete m_autojoin;
}

void SimpleSetupPage::displayIdentity(CertIdentity *id, CertIdentity *saveId)
{
    if (saveId) {
        saveToIdentity(saveId);
    }

    if (!id)
        return;
    qDebug() << "xxxxx SimpleSetupPage::displayIdentity realname = " << id->realName();
    m_realName->setText(id->realName());
    if (id->nicks().size())
        m_nickName->setText(id->nicks()[0]);
    if (id->nicks().size() > 1)
        m_alterNick->setText(id->nicks()[1]);
    m_awayMessage->setText(id->awayReason());
}

void SimpleSetupPage::saveToIdentity(CertIdentity *id)
{
    //QRegExp linebreaks = QRegExp("[\\r\\n]");
    id->setRealName(m_realName->text());
    QStringList nicks;
    if (m_nickName->text().size())
        nicks << m_nickName->text();
    if (m_alterNick->text().size())
        nicks << m_alterNick->text();
    id->setNicks(nicks);
    id->setAwayReason(m_awayMessage->text());
}

void SimpleSetupPage::displayNetworkInfo(const NetworkInfo &networkInfo)
{
    m_networkName->setText(networkInfo.networkName);
    Network::Server server;
    if (networkInfo.serverList.size())
        server = networkInfo.serverList[0];
    m_serverAddr->setText(server.host);
    m_port->setText(QString::number(server.port));
    m_password->setText(server.password);
    m_useSSL->setChecked(server.useSsl);
}

void SimpleSetupPage::setDefaultChannels(const QStringList& channels)
{
    qDebug() << "xxxxx SimpleSetupPage::setDefaultChannels defaultChannels " << channels;
    m_autojoin->setText(channels.join("\n"));
}

void SimpleSetupPage::saveToNetworkInfo(NetworkInfo &networkInfo)
{
    networkInfo.networkName = m_networkName->text();
    Network::ServerList list;
    Network::Server svr;
    svr.host = m_serverAddr->text();
    svr.port = m_port->text().toInt();
    svr.password = m_password->text();
    svr.useSsl = m_useSSL->isChecked();
    list << svr;
    networkInfo.serverList = list;
}

QStringList SimpleSetupPage::channels()
{
    return m_autojoin->text().split("\n",  QString::SkipEmptyParts);
}
