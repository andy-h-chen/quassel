#include "bb10ui.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/Application>
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

#include <QModelIndex>

#include "bb10uisettings.h"
#include "buffermodel.h"
#include "channellistdatamodel.h"
#include "chatlinemodel.h"
#include "client.h"
#include "clientbufferviewconfig.h"
#include "clientbufferviewmanager.h"
#include "clientidentity.h"
#include "simplesetuppage.h"
#include "uimessageprocessor.h"

Bb10Ui *Bb10Ui::s_instance = 0;

Bb10Ui::Bb10Ui(Application *app)
    : m_simpleSetupPage(0)
    , m_navPane(0)
    , m_identity(0)
{
    if (s_instance != 0) {
        qWarning() << "Bb10Ui has been instantiated again!";
        return;
    }
    s_instance = this;

    bool connectResult;
    Q_UNUSED(connectResult);

    m_navPane = new NavigationPane();

    app->setScene(m_navPane);
}

Bb10Ui::~Bb10Ui()
{
    if (m_simpleSetupPage)
        delete m_simpleSetupPage;
}

void Bb10Ui::init()
{
    m_chatListPage = Page::create();
    Container* container = new Container();
    DockLayout* chatListLayout = new DockLayout();
    container->setLayout(chatListLayout);
    m_chatListView = new ListView();
    container->add(m_chatListView);
    m_chatListPage->setContent(container);
    m_navPane->push(m_chatListPage);

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
    connect(Client::bufferViewManager(), SIGNAL(bufferViewConfigAdded(int)), this, SLOT(bufferViewConfigAdded(int)));
    connect(Client::bufferViewManager(), SIGNAL(bufferViewConfigDeleted(int)), this, SLOT(bufferViewConfigDeleted(int)));
    connect(Client::bufferViewManager(), SIGNAL(initDone()), this, SLOT(bufferViewManagerInitDone()));
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
    if (Client::identityIds().isEmpty()) {
        m_identity = new CertIdentity(-1, this);
        m_identity->setToDefaults();
        m_identity->setIdentityName(tr("Default Identity"));
    } else {
        m_identity = new CertIdentity(*Client::identity(Client::identityIds().first()), this);
    }

    QStringList defaultNets = Network::presetNetworks(true);
    QStringList defaultChannels;
    qDebug() << "xxxxx Bb10Ui::showNewIdentityPage defaultNets.isEmpty = " << defaultNets.isEmpty();
    if (!defaultNets.isEmpty()) {
        NetworkInfo info = Network::networkInfoFromPreset(defaultNets[0]);
        if (!info.networkName.isEmpty()) {
            m_networkInfo = info;
            qDebug() << "xxxxx Bb10Ui::showNewIdentityPage" << info.networkName << info.serverList.size();
            defaultChannels = Network::presetDefaultChannels(defaultNets[0]);
        }
    }

    Network::Server defaultServer;
    if (m_networkInfo.serverList.size()) {
        defaultServer = m_networkInfo.serverList[0];
    }
    if (!m_simpleSetupPage)
        m_simpleSetupPage = new SimpleSetupPage();
    m_navPane->push(m_simpleSetupPage->getPage());
    m_simpleSetupPage->displayIdentity(m_identity);
    m_simpleSetupPage->displayNetworkInfo(m_networkInfo);
    m_simpleSetupPage->setDefaultChannels(defaultChannels);
}

void Bb10Ui::saveIdentityAndServer()
{
    qDebug() << "xxxxx Bb10Ui::saveIdentityAndServer";
    m_simpleSetupPage->saveToIdentity(m_identity);
    if (m_identity->id().isValid()) {
        Client::updateIdentity(m_identity->id(), m_identity->toVariantMap());
        identityReady(m_identity->id());
    }
    else {
        connect(Client::instance(), SIGNAL(identityCreated(IdentityId)), this, SLOT(identityReady(IdentityId)));
        Client::createIdentity(*m_identity);
    }
    //m_identity->setRealName();
    m_navPane->pop();
}

void Bb10Ui::identityReady(IdentityId id)
{
    disconnect(Client::instance(), SIGNAL(identityCreated(IdentityId)), this, SLOT(identityReady(IdentityId)));
    NetworkInfo networkInfo = m_networkInfo;
    m_simpleSetupPage->saveToNetworkInfo(networkInfo);
    networkInfo.identity = id;
    connect(Client::instance(), SIGNAL(networkCreated(NetworkId)), this, SLOT(networkReady(NetworkId)));
    Client::createNetwork(networkInfo, m_simpleSetupPage->channels());
}

void Bb10Ui::networkReady(NetworkId id)
{
    qDebug() << "xxxxx Bb10Ui::networkReady " << id;
    disconnect(Client::instance(), SIGNAL(networkCreated(NetworkId)), this, SLOT(networkReady(NetworkId)));
    const Network *net = Client::network(id);
    Q_ASSERT(net);
    net->requestConnect();
}

void Bb10Ui::bufferViewConfigAdded(int bufferViewConfigId)
{
    qDebug() << "xxxxx Bb10Ui::bufferViewConfigAdde id = " << bufferViewConfigId;
    ClientBufferViewConfig *config = Client::bufferViewManager()->clientBufferViewConfig(bufferViewConfigId);
    qDebug() << "xxxxx Bb10Ui::bufferViewConfigAdde config->networkId = " << config->networkId().toInt() << " bufferViewName = " << config->bufferViewName() << "bufferList = " << config->bufferList().size() << config->bufferList();
    m_chatListView->setDataModel(new ChannelListDataModel());
}

void Bb10Ui::bufferViewConfigDeleted(int bufferViewConfigId)
{
    qDebug() << "xxxxx Bb10Ui::bufferViewConfigDeleted id = " << bufferViewConfigId;
}
void Bb10Ui::bufferViewManagerInitDone()
{
    qDebug() << "xxxxx bufferViewManagerInitDone";
}

AbstractMessageProcessor *Bb10Ui::createMessageProcessor(QObject *parent)
{
    return new UiMessageProcessor(parent);
}
